#include "state/serialize/StateHandlers.hh"
#include "state/serialize/PlayerHandlers.hh"
#include "state/serialize/ShopHandlers.hh"
#include "state/serialize/UnitHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"

namespace freeisle::state::serialize {

StateLoader::StateLoader(State &state, def::Scenario &scenario,
                         def::serialize::AuxData &aux)
    : state_(state), scenario_(scenario), aux_(aux) {}

void StateLoader::load(json::loader::Context &ctx, Json::Value &value) {

  def::serialize::CollectionLoader<Team, TeamLoader> teams(state_.teams);
  def::serialize::EmptyCollectionLoader<Player> emptyPlayers(state_.players);
  def::serialize::EmptyCollectionLoader<Unit> emptyUnits(state_.units);
  def::serialize::CollectionLoader<Shop, ShopLoader> shops(
      state_.shops, ShopLoader(scenario_.shops, state_.map, state_.players));
  UnitLoader unit_loader(scenario_.units, state_.map, state_.shops,
                         state_.units, state_.players);
  def::serialize::CollectionLoaderPass<Unit, UnitLoader> units(state_.units,
                                                               unit_loader);
  def::serialize::CollectionLoaderPass<Player, PlayerLoader> players(
      state_.players,
      PlayerLoader(scenario_.map, state_.teams, state_.units, aux_));

  state_.scenario = &scenario_;
  state_.map.def = &scenario_.map;
  state_.map.grid = core::Grid<Map::Hex>(scenario_.map.grid.width(),
                                         scenario_.map.grid.height());

  // Need to empty-load players and units first
  json::loader::load_object(ctx, value, "teams", teams);
  json::loader::load_object(ctx, value, "players", emptyPlayers);
  json::loader::load_object(ctx, value, "units", emptyUnits);
  json::loader::load_object(ctx, value, "shops", shops);
  json::loader::load_object(ctx, value, "units", units);
  json::loader::load_object(ctx, value, "players", players);
  state_.turn_num = json::loader::load<uint32_t>(ctx, value, "turn");
  state_.player_at_turn = def::serialize::load_mandatory_ref<Player>(
      ctx, value, "player_at_turn", state_.players);
}

StateSaver::StateSaver(const State &state, def::serialize::AuxData &aux)
    : state_(state), aux_(aux) {}

void StateSaver::save(json::saver::Context &ctx, Json::Value &value) {
  const def::Scenario &scenario = *state_.scenario;

  def::serialize::CollectionSaver<Team, TeamSaver> teams(state_.teams);
  def::serialize::CollectionSaver<Player, PlayerSaver> players(
      state_.players,
      PlayerSaver(scenario.map, state_.teams, state_.units, aux_));
  def::serialize::CollectionSaver<Shop, ShopSaver> shops(
      state_.shops, ShopSaver(scenario.shops, state_.players));
  def::serialize::CollectionSaver<Unit, UnitSaver> units(
      state_.units,
      UnitSaver(scenario.units, state_.shops, state_.units, state_.players));

  json::saver::save_object(ctx, value, "teams", teams);
  json::saver::save_object(ctx, value, "players", players);
  json::saver::save_object(ctx, value, "shops", shops);
  json::saver::save_object(ctx, value, "units", units);
  json::saver::save(ctx, value, "turn", state_.turn_num);
  def::serialize::save_ref(ctx, value, "player_at_turn", state_.player_at_turn,
                           state_.players);
}

} // namespace freeisle::state::serialize
