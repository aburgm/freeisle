#include "state/serialize/UnitHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"
#include "def/serialize/ShopDefHandlers.hh"
#include "def/serialize/UnitDefHandlers.hh"

#include <numeric>

namespace freeisle::state::serialize {

namespace {

class StatsLoader {
public:
  StatsLoader(Unit::Stats &stats);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  Unit::Stats &stats_;
};

class StatsSaver {
public:
  StatsSaver(const Unit::Stats &stats);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const Unit::Stats &stats_;
};

StatsLoader::StatsLoader(Unit::Stats &stats) : stats_(stats) {}

void StatsLoader::load(json::loader::Context &ctx, Json::Value &value) {
  stats_.hits_dealt = json::loader::load<uint32_t>(ctx, value, "hits_dealt");
  stats_.hits_taken = json::loader::load<uint32_t>(ctx, value, "hits_taken");

  stats_.damage_dealt =
      json::loader::load<uint32_t>(ctx, value, "damage_dealt");
  stats_.damage_taken =
      json::loader::load<uint32_t>(ctx, value, "damage_taken");

  stats_.hexes_moved = json::loader::load<uint32_t>(ctx, value, "hexes_moved");
}

StatsSaver::StatsSaver(const Unit::Stats &stats) : stats_(stats) {}

void StatsSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save(ctx, value, "hits_dealt", stats_.hits_dealt);
  json::saver::save(ctx, value, "hits_taken", stats_.hits_taken);
  json::saver::save(ctx, value, "damage_dealt", stats_.damage_dealt);
  json::saver::save(ctx, value, "damage_taken", stats_.damage_taken);
  json::saver::save(ctx, value, "hexes_moved", stats_.hexes_moved);
}

class ResupplyLoader {
public:
  ResupplyLoader(Unit &unit);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  Unit &unit_;
};

ResupplyLoader::ResupplyLoader(Unit &unit) : unit_(unit) {}

void ResupplyLoader::load(json::loader::Context &ctx, Json::Value &value) {
  def::serialize::ResupplyLoader loader(unit_.supplies);
  loader.load(ctx, value);

  if (unit_.supplies.fuel > unit_.def->supplies.fuel) {
    throw json::loader::Error::create(ctx, "fuel", value["fuel"],
                                      "Unit has more than maximum fuel supply");
  }

  if (unit_.supplies.repair > unit_.def->supplies.repair) {
    throw json::loader::Error::create(
        ctx, "repair", value["repair"],
        "Unit has more than maximum repair supply");
  }

  for (const core::EnumEntry<def::DamageType> damage_type : def::DamageTypes) {
    if (unit_.supplies.ammo[damage_type.value] >
        unit_.def->supplies.ammo[damage_type.value]) {
      throw json::loader::Error::create(
          ctx, fmt::format("ammo.{}", damage_type.str).c_str(),
          value["ammo"][damage_type.str],
          fmt::format("Unit has more than maximum ammo supply for {}",
                      damage_type.str));
    }
  }
}

class WeaponLoader {
public:
  void set(def::Ref<const def::WeaponDef> ref, uint32_t &ammo);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::NullableRef<const def::WeaponDef> ref_;
  uint32_t *ammo_ = nullptr;
};

class WeaponSaver {
public:
  void set(def::Ref<const def::WeaponDef> ref, uint32_t ammo);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  uint32_t ammo_;
};

void WeaponLoader::set(def::Ref<const def::WeaponDef> ref, uint32_t &ammo) {
  ref_ = ref;
  ammo_ = &ammo;
}

void WeaponLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(ammo_ != nullptr);

  *ammo_ = json::loader::load<uint32_t>(ctx, value, "ammo");

  if (*ammo_ > ref_->ammo) {
    throw json::loader::Error::create(ctx, "ammo", value["ammo"],
                                      "Weapon has more than maximum ammo");
  }
}

void WeaponSaver::set(def::Ref<const def::WeaponDef> ref, uint32_t ammo) {
  ammo_ = ammo;
}

void WeaponSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save(ctx, value, "ammo", ammo_);
}

} // namespace

UnitLoader::UnitLoader(def::Collection<def::UnitDef> &unit_defs, Map &map,
                       def::Collection<Shop> &shops,
                       def::Collection<Unit> &units,
                       def::Collection<Player> &players)
    : unit_defs_(unit_defs), map_(map), shops_(shops), units_(units),
      players_(players) {}

void UnitLoader::set(def::Ref<Unit> unit) { unit_ = unit; }

void UnitLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(unit_);

  def::serialize::LocationLoader location(unit_->location, *map_.def);
  StatsLoader stats(unit_->stats);
  ResupplyLoader supplies(*unit_);

  unit_->def =
      def::serialize::load_mandatory_ref(ctx, value, "def", unit_defs_);
  def::serialize::RefMapLoader<def::WeaponDef, uint32_t, WeaponLoader> weapons(
      unit_->ammo, unit_->def->weapons);

  unit_->owner =
      def::serialize::load_nullable_ref(ctx, value, "owner", players_);
  json::loader::load_object(ctx, value, "location", location);
  unit_->health = json::loader::load<uint32_t>(ctx, value, "health");
  if (unit_->health > 100) {
    throw json::loader::Error::create(ctx, "health", value["health"],
                                      "Unit has more than maximum health");
  }

  unit_->level = json::loader::load_enum(ctx, value, "level", def::Levels);
  if (unit_->level != unit_->def->level) {
    if (!unit_->def->caps.is_set(def::UnitDef::Cap::Soar)) {
      throw json::loader::Error::create(ctx, "level", value["level"],
                                        "Unit level does not match unit def");
    }

    if ((unit_->def->level == def::Level::Air &&
         unit_->level != def::Level::HighAir) ||
        (unit_->def->level == def::Level::Water &&
         unit_->level != def::Level::UnderWater)) {
      throw json::loader::Error::create(ctx, "level", value["level"],
                                        "Unit level does not match unit def");
    }
  }

  unit_->movement = json::loader::load<uint32_t>(ctx, value, "movement");
  if (unit_->movement > unit_->def->movement) {
    throw json::loader::Error::create(ctx, "movement", value["movement"],
                                      "Unit has more than maximum movement");
  }

  unit_->fuel = json::loader::load<uint32_t>(ctx, value, "fuel");
  if (unit_->fuel > unit_->def->fuel) {
    throw json::loader::Error::create(ctx, "fuel", value["fuel"],
                                      "Unit has more than maximum fuel");
  }

  unit_->experience = json::loader::load<uint32_t>(ctx, value, "experience");
  unit_->has_actioned = json::loader::load<bool>(ctx, value, "has_actioned");
  unit_->has_soared = json::loader::load<bool>(ctx, value, "has_soared");
  json::loader::load_object(ctx, value, "supplies", supplies);
  json::loader::load_object(ctx, value, "weapons", weapons);

  // Contained units are populated when contained units are loaded.
  unit_->container.def = &unit_->def->container;

  unit_->contained_in_unit = def::serialize::load_nullable_ref<Unit>(
      ctx, value, "contained_in_unit", units_);
  unit_->contained_in_shop = def::serialize::load_nullable_ref<Shop>(
      ctx, value, "contained_in_shop", shops_);
  json::loader::load_object(ctx, value, "stats", stats);

  if (unit_->contained_in_shop) {
    unit_->contained_in_shop->container.units.push_back(unit_);
  } else if (unit_->contained_in_unit) {
    unit_->contained_in_unit->container.units.push_back(unit_);
  } else {
    // should have been checked when loading location
    const def::Location &location = unit_->location;
    assert(location.x <= map_.grid.width() && location.y <= map_.grid.height());

    // TODO(armin): check whether unit can be on this hex in the first place
    // based on movement_cost array... (and level?)
    if (unit_->level == def::Level::UnderWater) {
      if (map_.grid(location.x, location.y).shop ||
          map_.grid(location.x, location.y).subsurface_unit) {
        const std::string message = fmt::format(
            "Location x={}, y={} is already occupied", location.x, location.y);
        throw json::loader::Error::create(ctx, "location", value["location"],
                                          message);
      }

      map_.grid(location.x, location.y).subsurface_unit = unit_;
    } else {
      if (map_.grid(location.x, location.y).shop ||
          map_.grid(location.x, location.y).surface_unit) {
        const std::string message = fmt::format(
            "Location x={}, y={} is already occupied", location.x, location.y);
        throw json::loader::Error::create(ctx, "location", value["location"],
                                          message);
      }

      map_.grid(location.x, location.y).surface_unit = unit_;
    }
  }

  check_unit_container(ctx, unit_, value);
}

void UnitLoader::check_unit_container(json::loader::Context &ctx,
                                      def::Ref<Unit> unit, Json::Value &value) {
  if (unit_->contained_in_shop && unit_->contained_in_unit) {
    throw json::loader::Error::create(
        ctx, "contained_in_shop", value["contained_in_shop"],
        "Unit cannot be contained in both unit and shop");
  }

  if (unit_->contained_in_unit && unit_->contained_in_unit == unit_) {
    throw json::loader::Error::create(ctx, "contained_in_unit",
                                      value["contained_in_unit"],
                                      "Unit cannot be contained in itself");
  }

  check_container(ctx, value, unit->contained_in_shop, unit->contained_in_unit,
                  unit->owner, unit->location, unit->container,
                  unit->def->container);
}

void UnitLoader::check_container(json::loader::Context &ctx, Json::Value &value,
                                 const def::NullableRef<Shop> &parent_shop,
                                 const def::NullableRef<Unit> &parent_unit,
                                 const def::NullableRef<Player> &owner,
                                 const def::Location &container_location,
                                 const Container &container,
                                 const def::ContainerDef &def) {
  if (parent_shop) {
    check_container(ctx, value, def::NullableRef<Shop>(),
                    def::NullableRef<Unit>(), parent_shop->owner,
                    parent_shop->def->location, parent_shop->container,
                    parent_shop->def->container);
  }

  if (parent_unit && parent_unit->def) {
    check_container(ctx, value, parent_unit->contained_in_shop,
                    parent_unit->contained_in_unit, parent_unit->owner,
                    parent_unit->location, parent_unit->container,
                    parent_unit->def->container);
  }

  // TODO(armin): provide better error messages here; ideally it always points
  // to the container unit or shop and lists all object IDs of units contained.
  if (container.units.size() > def.max_units) {
    throw json::loader::Error::create(
        ctx, "", value,
        "Container has more units contained than allowed by def");
  }

  for (const def::Ref<Unit> &unit : container.units) {
    if (unit->owner != owner) {
      const std::string message = fmt::format(
          "Contained unit \"{}\" has different owner than container",
          unit.id());
      throw json::loader::Error::create(ctx, "", value, message);
    }

    if (!def.supported_levels.is_set(unit->level)) {
      const std::string message = fmt::format(
          "Contained unit \"{}\" has level not supported by container",
          unit.id());
      throw json::loader::Error::create(ctx, "", value, message);
    }

    if (unit->location.x != container_location.x ||
        unit->location.y != container_location.y) {
      const std::string message = fmt::format(
          "Contained unit \"{}\" location does not match container location",
          unit.id());
      throw json::loader::Error::create(ctx, "", value, message);
    }

    if (!unit->container.units.empty()) {
      const std::string message =
          fmt::format("Contained unit \"{}\" has other unit \"{}\" contained",
                      unit.id(), unit->container.units.front().id());
      throw json::loader::Error::create(ctx, "", value, message);
    }
  }

  const uint32_t weight =
      std::accumulate(container.units.begin(), container.units.end(), 0,
                      [](uint32_t val, const def::Ref<Unit> &unit) {
                        return val + unit->def->weight;
                      });

  if (weight > def.max_weight) {
    throw json::loader::Error::create(
        ctx, "", value,
        "Contained units exceed maximum weight allowed by container def");
  }
}

UnitSaver::UnitSaver(const def::Collection<def::UnitDef> &unit_defs,
                     const def::Collection<Shop> &shops,
                     const def::Collection<Unit> &units,
                     const def::Collection<Player> &players)
    : unit_(nullptr), unit_defs_(unit_defs), shops_(shops), units_(units),
      players_(players) {}

void UnitSaver::set(def::Ref<const Unit> unit) { unit_ = &*unit; }

void UnitSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(unit_ != nullptr);

  def::serialize::LocationSaver location(unit_->location);
  StatsSaver stats(unit_->stats);
  def::serialize::ResupplySaver supplies(unit_->supplies);
  def::serialize::RefMapSaver<def::WeaponDef, uint32_t, WeaponSaver> weapons(
      unit_->ammo, unit_->def->weapons);

  def::serialize::save_ref(ctx, value, "def", unit_->def, unit_defs_);
  def::serialize::save_ref(ctx, value, "owner", unit_->owner, players_);
  json::saver::save_object(ctx, value, "location", location);
  json::saver::save(ctx, value, "health", unit_->health);
  json::saver::save_enum(ctx, value, "level", unit_->level, def::Levels);
  json::saver::save(ctx, value, "movement", unit_->movement);
  json::saver::save(ctx, value, "fuel", unit_->fuel);
  json::saver::save(ctx, value, "experience", unit_->experience);
  json::saver::save(ctx, value, "has_actioned", unit_->has_actioned);
  json::saver::save(ctx, value, "has_soared", unit_->has_soared);
  json::saver::save_object(ctx, value, "supplies", supplies);
  json::saver::save_object(ctx, value, "weapons", weapons);

  // Contained units are not saved in this unit, the reference is stored
  // in the units being contained instead.

  def::serialize::save_ref(ctx, value, "contained_in_unit",
                           unit_->contained_in_unit, units_);
  def::serialize::save_ref(ctx, value, "contained_in_shop",
                           unit_->contained_in_shop, shops_);
  json::saver::save_object(ctx, value, "stats", stats);
}

} // namespace freeisle::state::serialize
