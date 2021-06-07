#include "state/serialize/ShopDefHandlers.hh"

#include "state/serialize/CollectionLoaders.hh"
#include "state/serialize/CollectionSavers.hh"
#include "state/serialize/UnitDefHandlers.hh"

#include <fmt/format.h>

namespace freeisle::state::serialize {

LocationLoader::LocationLoader(def::Location &loc, const def::MapDef &map)
    : loc_(loc), map_(map) {}

void LocationLoader::load(json::loader::Context &ctx, Json::Value &value) {
  loc_.x = json::loader::load<uint32_t>(ctx, value, "x");
  loc_.y = json::loader::load<uint32_t>(ctx, value, "y");

  if (loc_.x >= map_.grid.width() || loc_.y >= map_.grid.height()) {
    const std::string message =
        fmt::format("Location is out of bounds; loc={},{} but bounds={}x{}",
                    loc_.x, loc_.y, map_.grid.width(), map_.grid.height());
    throw json::loader::Error::create(ctx, "", value, message);
  }
}

LocationSaver::LocationSaver(const def::Location &loc) : loc_(loc) {}

void LocationSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save(ctx, value, "x", loc_.x);
  json::saver::save(ctx, value, "y", loc_.y);
}

ShopDefLoader::ShopDefLoader(const def::MapDef &map,
                             def::Collection<def::UnitDef> &unit_defs,
                             AuxData &aux)
    : shop_(nullptr), map_(map), unit_defs_(unit_defs), aux_(aux) {}

void ShopDefLoader::set(def::ShopDef &shop) { shop_ = &shop; }

void ShopDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(shop_ != nullptr);

  ContainerDefLoader container(shop_->container);
  LocationLoader location(shop_->location, map_);

  shop_->name = json::loader::load<std::string>(ctx, value, "name");
  shop_->type = json::loader::load_enum(ctx, value, "type", def::ShopDefTypes);
  shop_->income = json::loader::load<uint32_t>(ctx, value, "income");
  json::loader::load_object(ctx, value, "container", container);
  shop_->production_list =
      load_ref_set(ctx, value, "production_list", unit_defs_);
  json::loader::load_object(ctx, value, "location", location);
}

ShopDefSaver::ShopDefSaver(const def::Collection<def::UnitDef> &unit_defs,
                           AuxData &aux)
    : shop_(nullptr), unit_defs_(unit_defs), aux_(aux) {}

void ShopDefSaver::set(const def::ShopDef &shop) { shop_ = &shop; }

void ShopDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(shop_ != nullptr);

  ContainerDefSaver container(shop_->container);
  LocationSaver location(shop_->location);

  json::saver::save(ctx, value, "name", shop_->name);
  json::saver::save_enum(ctx, value, "type", shop_->type, def::ShopDefTypes);
  json::saver::save(ctx, value, "income", shop_->income);
  json::saver::save_object(ctx, value, "container", container);
  save_ref_set(ctx, value, "production_list", shop_->production_list,
               unit_defs_);
  json::saver::save_object(ctx, value, "location", location);
}

} // namespace freeisle::state::serialize
