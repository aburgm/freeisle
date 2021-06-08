#include "def/serialize/ScenarioHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"
#include "def/serialize/MapDefHandlers.hh"
#include "def/serialize/ShopDefHandlers.hh"
#include "def/serialize/UnitDefHandlers.hh"

#include <fmt/format.h>

namespace freeisle::def::serialize {

ScenarioLoader::ScenarioLoader(def::Scenario &scenario, AuxData &aux)
    : scenario_(&scenario), aux_(aux) {}

void ScenarioLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(scenario_ != nullptr);

  MapDefLoader map_loader(scenario_->map, aux_);
  CollectionLoader<def::UnitDef, UnitDefLoader> unit_loader(scenario_->units,
                                                            aux_);
  CollectionLoader<def::ShopDef, ShopDefLoader> shop_loader(
      scenario_->shops, ShopDefLoader(scenario_->map, scenario_->units, aux_));

  scenario_->name = json::loader::load<std::string>(ctx, value, "name");
  scenario_->description =
      json::loader::load<std::string>(ctx, value, "description");
  json::loader::load_object(ctx, value, "map", map_loader);
  json::loader::load_object(ctx, value, "units", unit_loader);
  json::loader::load_object(ctx, value, "shops", shop_loader);
}

ScenarioSaver::ScenarioSaver(const def::Scenario &scenario, AuxData &aux,
                             const std::string &map_filename)
    : scenario_(&scenario), aux_(aux), map_filename_(map_filename) {}

void ScenarioSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(scenario_ != nullptr);

  MapDefSaver map_saver(scenario_->map, aux_, map_filename_);
  CollectionSaver<def::UnitDef, UnitDefSaver> unit_saver(scenario_->units,
                                                         aux_);
  CollectionSaver<def::ShopDef, ShopDefSaver> shop_saver(
      scenario_->shops, ShopDefSaver(scenario_->units, aux_));

  json::saver::save(ctx, value, "name", scenario_->name);
  json::saver::save(ctx, value, "description", scenario_->description);
  json::saver::save_object(ctx, value, "map", map_saver);
  json::saver::save_object(ctx, value, "units", unit_saver);
  json::saver::save_object(ctx, value, "shops", shop_saver);
}

} // namespace freeisle::def::serialize
