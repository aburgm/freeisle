#include "state/serialize/PlayerHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"

namespace freeisle::state::serialize {

ColorLoader::ColorLoader(freeisle::core::color::Rgb8 &color) : color_(color) {}

void ColorLoader::load(json::loader::Context &ctx, Json::Value &value) {
  color_.r = json::loader::load<uint8_t>(ctx, value, "red");
  color_.g = json::loader::load<uint8_t>(ctx, value, "green");
  color_.b = json::loader::load<uint8_t>(ctx, value, "blue");
}

ColorSaver::ColorSaver(const freeisle::core::color::Rgb8 &color)
    : color_(color) {}

void ColorSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save<uint8_t>(ctx, value, "red", color_.r);
  json::saver::save<uint8_t>(ctx, value, "green", color_.g);
  json::saver::save<uint8_t>(ctx, value, "blue", color_.b);
}

TeamLoader::TeamLoader() : team_(nullptr) {}

void TeamLoader::set(state::Team &team) { team_ = &team; }

void TeamLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(team_ != nullptr);

  team_->name = json::loader::load<std::string>(ctx, value, "name");
}

TeamSaver::TeamSaver() : team_(nullptr) {}

void TeamSaver::set(const state::Team &team) { team_ = &team; }

void TeamSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(team_ != nullptr);

  json::saver::save(ctx, value, "name", team_->name);
}

PlayerLoader::PlayerLoader(const def::MapDef &map, def::Collection<Team> &teams,
                           def::Collection<Unit> &units,
                           def::serialize::AuxData &aux)
    : player_(nullptr), aux_(aux), map_(map), teams_(teams), units_(units) {}

void PlayerLoader::set(state::Player &player) { player_ = &player; }

void PlayerLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(player_ != nullptr);

  ColorLoader color(player_->color);

  player_->name = json::loader::load<std::string>(ctx, value, "name");
  json::loader::load_object(ctx, value, "color", color);
  player_->team = def::serialize::load_nullable_ref(ctx, value, "team", teams_);

  const uint32_t expected_size =
      (map_.grid.width() * map_.grid.height() + 7) / 8;
  const std::vector<uint8_t> fow = json::loader::load_binary(ctx, value, "fow");
  if (fow.size() != expected_size) {
    const std::string message = fmt::format(
        "FoW field does not have expected length; got {} but expected {}",
        fow.size(), expected_size);
    throw json::loader::Error::create(ctx, "fow", value["fow"], message);
  }

  player_->fow = core::Grid<Player::Fow>(map_.grid.width(), map_.grid.height());
  for (uint32_t y = 0; y < map_.grid.height(); ++y) {
    for (uint32_t x = 0; x < map_.grid.width(); ++x) {
      uint32_t index = y * map_.grid.width() + x;
      assert(index / 8 < fow.size());

      player_->fow(x, y).discovered = fow[index / 8] & (1 << (index % 8));
      player_->fow(x, y).view = 0; // updated in post-load
    }
  }

  player_->wealth = json::loader::load<uint32_t>(ctx, value, "wealth");
  player_->captain =
      def::serialize::load_nullable_ref(ctx, value, "captain", units_);
  if (player_->captain) {
    if (!player_->captain->owner || &*player_->captain->owner != player_) {
      throw json::loader::Error::create(
          ctx, "captain", value["captain"],
          "Unit declared as captain has different owner");
    }
  }

  player_->lose_conditions =
      json::loader::load_bitmask(ctx, value, "lose_conditions", def::Goals);
  player_->is_eliminated =
      json::loader::load<bool>(ctx, value, "is_eliminated");

  for (def::Collection<Unit>::iterator iter = units_.begin();
       iter != units_.end(); ++iter) {
    if (iter->second.owner && &*iter->second.owner == player_) {
      player_->units.insert(iter);
    }
  }
}

PlayerSaver::PlayerSaver(const def::MapDef &map,
                         const def::Collection<Team> &teams,
                         const def::Collection<Unit> &units,
                         def::serialize::AuxData &aux)
    : player_(nullptr), aux_(aux), map_(map), teams_(teams), units_(units),
      player_index_(0) {}

void PlayerSaver::set(const state::Player &player) {
  player_ = &player;
  ++player_index_;
}

void PlayerSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(player_ != nullptr);

  ColorSaver color(player_->color);

  json::saver::save<std::string>(ctx, value, "name", player_->name);
  json::saver::save_object(ctx, value, "color", color);
  def::serialize::save_ref(ctx, value, "team", player_->team, teams_);

  const uint32_t fow_array_size =
      (player_->fow.width() * player_->fow.height() + 7) / 8;
  std::vector<uint8_t> fow_array(fow_array_size);

  for (uint32_t y = 0; y < player_->fow.height(); ++y) {
    for (uint32_t x = 0; x < player_->fow.width(); ++x) {
      uint32_t index = y * map_.grid.width() + x;
      assert(index / 8 < fow_array.size());

      if (player_->fow(x, y).discovered) {
        fow_array[index / 8] |= (1 << (index % 8));
      }
    }
  }

  json::saver::save_binary(ctx, value, "fow", fow_array.data(),
                           fow_array.size(),
                           fmt::format("fow.{}.bin", player_index_).c_str());
  json::saver::save(ctx, value, "wealth", player_->wealth);

  def::serialize::save_ref(ctx, value, "captain", player_->captain, units_);

  json::saver::save_bitmask(ctx, value, "lose_conditions",
                            player_->lose_conditions, def::Goals);
  json::saver::save(ctx, value, "is_eliminated", player_->is_eliminated);

  // unit list is not saved; it is reconstructed from global unit list on load
}

} // namespace freeisle::state::serialize
