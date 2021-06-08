#include "def/serialize/ScenarioHandlers.hh"

#include "fs/Path.hh"

#include "fs/test/util/TempDirFixture.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestScenarioHandlers : public ::freeisle::fs::test::TempDirFixture {
public:
  TestScenarioHandlers() : aux{system.logger} {}

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;
};

TEST_F(TestScenarioHandlers, Load) {
  freeisle::def::Scenario scenario;
  ;
  freeisle::def::serialize::ScenarioLoader loader(scenario, aux);

  freeisle::json::loader::load_root_object(
      freeisle::fs::path::join(orig_directory, "data", "scenario.json").c_str(),
      loader);

  EXPECT_EQ(scenario.name, "My scenario");
  EXPECT_EQ(scenario.description, "Scenario description");
  EXPECT_EQ(scenario.map.grid.width(), 5);
  EXPECT_EQ(scenario.map.grid.height(), 5);
  EXPECT_EQ(scenario.units.size(), 1);
  EXPECT_EQ(scenario.units.count("grunt"), 1);
  EXPECT_EQ(scenario.shops.size(), 1);
  EXPECT_EQ(scenario.shops.count("fayetteville"), 1);
}

TEST_F(TestScenarioHandlers, Save) {
  const freeisle::def::Scenario scenario{
      .name = "My scenario",
      .description = "Scenario description",
      .map = {.decoration_defs =
                  {{"obj001", freeisle::def::DecorationDef{.name = "flowers"}}},
              .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)},
      .units = {{"grunt", freeisle::def::UnitDef{}}},
      .shops = {{"fayetteville", freeisle::def::ShopDef{}}}};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {".units.grunt",
       freeisle::json::IncludeInfo{.filename = "unit_grunt.json"}},
      {".shops.fayetteville",
       freeisle::json::IncludeInfo{.filename = "shop_fayetteville.json"}}};

  freeisle::def::serialize::ScenarioSaver saver(scenario, aux, "empty_5x5.png");
  freeisle::json::saver::save_root_object("saved.json", saver, &include_map);

  const std::vector<uint8_t> result =
      freeisle::fs::read_file("saved.json", nullptr);
  const std::vector<uint8_t> expected = freeisle::fs::read_file(
      freeisle::fs::path::join(orig_directory, "data", "scenario.json").c_str(),
      nullptr);

  freeisle::json::test::check(result, expected);
}
