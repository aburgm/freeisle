#include "def/serialize/UnitDefHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"

namespace freeisle::def::serialize {

void ResupplyLoader::load(json::loader::Context &ctx, Json::Value &value) {
  json::loader::EnumMapLoader<uint32_t, def::DamageType> ammo(
      resupply_.ammo, def::DamageTypeNames);

  resupply_.fuel = json::loader::load<uint32_t>(ctx, value, "fuel");
  resupply_.repair = json::loader::load<uint32_t>(ctx, value, "repair");
  json::loader::load_object(ctx, value, "ammo", ammo);
}

void ResupplySaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::EnumMapSaver<uint32_t, def::DamageType> ammo(
      resupply_.ammo, def::DamageTypeNames);

  json::saver::save(ctx, value, "fuel", resupply_.fuel);
  json::saver::save(ctx, value, "repair", resupply_.repair);
  json::saver::save_object(ctx, value, "ammo", ammo);
}

void ContainerDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  container_.max_units = json::loader::load<uint32_t>(ctx, value, "max_units");
  container_.max_weight =
      json::loader::load<uint32_t>(ctx, value, "max_weight");
  container_.supported_levels = json::loader::load_bitmask<def::Level>(
      ctx, value, "supported_levels", def::Levels);
}

void ContainerDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save(ctx, value, "max_units", container_.max_units);
  json::saver::save(ctx, value, "max_weight", container_.max_weight);
  json::saver::save_bitmask(ctx, value, "supported_levels",
                            container_.supported_levels, def::Levels);
}

void WeaponDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(weapon_ != nullptr);

  weapon_->name = json::loader::load<std::string>(ctx, value, "name");
  weapon_->damage_type = json::loader::load_enum<def::DamageType>(
      ctx, value, "damage_type", def::DamageTypes);
  weapon_->damage = json::loader::load<uint32_t>(ctx, value, "damage");
  weapon_->min_range = json::loader::load<uint32_t>(ctx, value, "min_range");
  weapon_->max_range = json::loader::load<uint32_t>(ctx, value, "max_range");
  weapon_->ammo = json::loader::load<uint32_t>(ctx, value, "ammo");
}

void WeaponDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(weapon_ != nullptr);

  json::saver::save(ctx, value, "name", weapon_->name);
  json::saver::save_enum(ctx, value, "damage_type", weapon_->damage_type,
                         def::DamageTypes);
  json::saver::save(ctx, value, "damage", weapon_->damage);
  json::saver::save(ctx, value, "min_range", weapon_->min_range);
  json::saver::save(ctx, value, "max_range", weapon_->max_range);
  json::saver::save(ctx, value, "ammo", weapon_->ammo);
}

void UnitDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(unit_ != nullptr);

  json::loader::EnumMapLoader<uint32_t, def::BaseTerrainType,
                              def::OverlayTerrainType>
      movement_cost(unit_->movement_cost, def::TerrainTypeNames);
  json::loader::EnumMapLoader<uint32_t, def::BaseTerrainType,
                              def::OverlayTerrainType>
      protection(unit_->protection, def::TerrainTypeNames);
  json::loader::EnumMapLoader<uint32_t, def::DamageType> resistance(
      unit_->resistance, def::DamageTypeNames);
  ResupplyLoader supplies(unit_->supplies);
  CollectionLoader<def::WeaponDef, WeaponDefLoader> weapons(unit_->weapons);
  ContainerDefLoader container(unit_->container);

  unit_->name = json::loader::load<std::string>(ctx, value, "name");
  unit_->description =
      json::loader::load<std::string>(ctx, value, "description");
  unit_->level = json::loader::load_enum(ctx, value, "level", def::Levels);
  unit_->caps =
      json::loader::load_bitmask(ctx, value, "caps", def::UnitDefCaps);
  unit_->armor = json::loader::load<uint32_t>(ctx, value, "armor");
  unit_->movement = json::loader::load<uint32_t>(ctx, value, "movement");
  unit_->fuel = json::loader::load<uint32_t>(ctx, value, "fuel");
  unit_->weight = json::loader::load<uint32_t>(ctx, value, "weight");
  json::loader::load_object(ctx, value, "movement_cost", movement_cost);
  json::loader::load_object(ctx, value, "protection", protection);
  json::loader::load_object(ctx, value, "resistance", resistance);
  json::loader::load_object(ctx, value, "supplies", supplies);
  json::loader::load_object(ctx, value, "weapons", weapons);
  json::loader::load_object(ctx, value, "container", container);
  unit_->value = json::loader::load<uint32_t>(ctx, value, "value");
  unit_->view_range = json::loader::load<uint32_t>(ctx, value, "view_range");
  unit_->jamming_range =
      json::loader::load<uint32_t>(ctx, value, "jamming_range");
}

void UnitDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(unit_ != nullptr);

  json::saver::EnumMapSaver<uint32_t, def::BaseTerrainType,
                            def::OverlayTerrainType>
      movement_cost(unit_->movement_cost, def::TerrainTypeNames);
  json::saver::EnumMapSaver<uint32_t, def::BaseTerrainType,
                            def::OverlayTerrainType>
      protection(unit_->protection, def::TerrainTypeNames);
  json::saver::EnumMapSaver<uint32_t, def::DamageType> resistance(
      unit_->resistance, def::DamageTypeNames);
  ResupplySaver supplies(unit_->supplies);
  CollectionSaver<def::WeaponDef, WeaponDefSaver> weapons(unit_->weapons);
  ContainerDefSaver container(unit_->container);

  json::saver::save(ctx, value, "name", unit_->name);
  json::saver::save(ctx, value, "description", unit_->description);
  json::saver::save_enum(ctx, value, "level", unit_->level, def::Levels);
  json::saver::save_bitmask(ctx, value, "caps", unit_->caps, def::UnitDefCaps);
  json::saver::save(ctx, value, "armor", unit_->armor);
  json::saver::save(ctx, value, "movement", unit_->movement);
  json::saver::save(ctx, value, "fuel", unit_->fuel);
  json::saver::save(ctx, value, "weight", unit_->weight);
  json::saver::save_object(ctx, value, "movement_cost", movement_cost);
  json::saver::save_object(ctx, value, "protection", protection);
  json::saver::save_object(ctx, value, "resistance", resistance);
  json::saver::save_object(ctx, value, "supplies", supplies);
  json::saver::save_object(ctx, value, "weapons", weapons);
  json::saver::save_object(ctx, value, "container", container);
  json::saver::save(ctx, value, "value", unit_->value);
  json::saver::save(ctx, value, "view_range", unit_->view_range);
  json::saver::save(ctx, value, "jamming_range", unit_->jamming_range);
}

} // namespace freeisle::def::serialize
