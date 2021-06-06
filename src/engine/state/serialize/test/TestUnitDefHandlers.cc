#include "state/serialize/UnitDefHandlers.hh"

#include "core/test/util/Util.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

// There's not too much to test: the complex functionality is in the helper
// utilities for loading and saving which have their own tests, in particular
// all failure cases.

class TestUnitDefHandlers : public ::testing::Test {
public:
  TestUnitDefHandlers() : aux{system.logger} {}

  freeisle::log::test::System system;
  freeisle::state::serialize::AuxData aux;
};

TEST_F(TestUnitDefHandlers, Load) {
  const std::string to_be_loaded = "{\"name\": \"grunt\",\n"
                                   " \"description\": \"Some description\",\n"
                                   " \"level\": \"land\",\n"
                                   " \"caps\": [\"capture\"],\n"
                                   " \"armor\": 250,\n"
                                   " \"movement\": 500,\n"
                                   " \"fuel\": 35,\n"
                                   " \"weight\": 100,\n"
                                   " \"movement_cost\": {\n"
                                   "   \"grass\": 100,\n"
                                   "   \"desert\": 150,\n"
                                   "   \"snow\": 250,\n"
                                   "   \"shallow_water\": 0,\n"
                                   "   \"deep_water\": 0,\n"
                                   "   \"hill\": 200,\n"
                                   "   \"mountain\": 300,\n"
                                   "   \"forest\": 150,\n"
                                   "   \"road\": 90,\n"
                                   "   \"crevice\": 250,\n"
                                   "   \"fortification\": 100\n"
                                   " },\n"
                                   " \"protection\": {\n"
                                   "   \"grass\": 100,\n"
                                   "   \"desert\": 100,\n"
                                   "   \"snow\": 100,\n"
                                   "   \"shallow_water\": 100,\n"
                                   "   \"deep_water\": 100,\n"
                                   "   \"hill\": 150,\n"
                                   "   \"mountain\": 200,\n"
                                   "   \"forest\": 125,\n"
                                   "   \"road\": 100,\n"
                                   "   \"crevice\": 125,\n"
                                   "   \"fortification\": 200\n"
                                   " },\n"
                                   " \"resistance\": {\n"
                                   "   \"small_caliber\": 100,\n"
                                   "   \"big_caliber\": 100,\n"
                                   "   \"missile\": 100,\n"
                                   "   \"energy\": 100,\n"
                                   "   \"explosive\": 80\n"
                                   " },\n"
                                   " \"supplies\": {\n"
                                   "   \"fuel\": 0,\n"
                                   "   \"repair\": 0,\n"
                                   "   \"ammo\": {\n"
                                   "     \"small_caliber\": 0,\n"
                                   "     \"big_caliber\": 0,\n"
                                   "     \"missile\": 0,\n"
                                   "     \"energy\": 0,\n"
                                   "     \"explosive\": 0\n"
                                   "   }\n"
                                   " },\n"
                                   " \"weapons\": {\n"
                                   "   \"rifle\": {\n"
                                   "     \"name\": \"Rifle\",\n"
                                   "     \"damage_type\": \"small_caliber\",\n"
                                   "     \"damage\": 180,\n"
                                   "     \"min_range\": 1,\n"
                                   "     \"max_range\": 1,\n"
                                   "     \"ammo\": 6\n"
                                   "   }\n"
                                   " },\n"
                                   " \"container\": {\n"
                                   "   \"max_units\": 0,\n"
                                   "   \"max_weight\": 0,\n"
                                   "   \"supported_levels\": []\n"
                                   " },\n"
                                   " \"value\": 800,\n"
                                   " \"view_range\": 4,\n"
                                   " \"jamming_range\": 1\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::UnitDef unit;
  freeisle::state::serialize::UnitDefLoader loader(aux);
  loader.set(unit);
  freeisle::json::loader::load_root_object(data, loader);

  EXPECT_EQ(unit.name, "grunt");
  EXPECT_EQ(unit.description, "Some description");
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.caps, freeisle::def::UnitDef::Cap::Capture);
  EXPECT_EQ(unit.armor, 250);
  EXPECT_EQ(unit.movement, 500);
  EXPECT_EQ(unit.fuel, 35);
  EXPECT_EQ(unit.weight, 100);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::Grass], 100);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::Desert], 150);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::Snow], 250);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::ShallowWater],
            0);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::DeepWater], 0);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::Hill], 200);
  EXPECT_EQ(unit.movement_cost[freeisle::def::BaseTerrainType::Mountain], 300);
  EXPECT_EQ(unit.movement_cost[freeisle::def::OverlayTerrainType::Forest], 150);
  EXPECT_EQ(unit.movement_cost[freeisle::def::OverlayTerrainType::Road], 90);
  EXPECT_EQ(unit.movement_cost[freeisle::def::OverlayTerrainType::Crevice],
            250);
  EXPECT_EQ(
      unit.movement_cost[freeisle::def::OverlayTerrainType::Fortification],
      100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::Grass], 100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::Desert], 100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::Snow], 100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::ShallowWater], 100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::DeepWater], 100);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::Hill], 150);
  EXPECT_EQ(unit.protection[freeisle::def::BaseTerrainType::Mountain], 200);
  EXPECT_EQ(unit.protection[freeisle::def::OverlayTerrainType::Forest], 125);
  EXPECT_EQ(unit.protection[freeisle::def::OverlayTerrainType::Road], 100);
  EXPECT_EQ(unit.protection[freeisle::def::OverlayTerrainType::Crevice], 125);
  EXPECT_EQ(unit.protection[freeisle::def::OverlayTerrainType::Fortification],
            200);
  EXPECT_EQ(unit.resistance[freeisle::def::DamageType::SmallCaliber], 100);
  EXPECT_EQ(unit.resistance[freeisle::def::DamageType::BigCaliber], 100);
  EXPECT_EQ(unit.resistance[freeisle::def::DamageType::Missile], 100);
  EXPECT_EQ(unit.resistance[freeisle::def::DamageType::Energy], 100);
  EXPECT_EQ(unit.resistance[freeisle::def::DamageType::Explosive], 80);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.weapons.size(), 1);

  const freeisle::def::Collection<freeisle::def::WeaponDef>::const_iterator
      iter = unit.weapons.find("rifle");
  ASSERT_NE(iter, unit.weapons.end());
  EXPECT_EQ(iter->second.name, "Rifle");
  EXPECT_EQ(iter->second.damage_type, freeisle::def::DamageType::SmallCaliber);
  EXPECT_EQ(iter->second.damage, 180);
  EXPECT_EQ(iter->second.min_range, 1);
  EXPECT_EQ(iter->second.max_range, 1);
  EXPECT_EQ(iter->second.ammo, 6);

  EXPECT_EQ(unit.container.max_units, 0);
  EXPECT_EQ(unit.container.max_weight, 0);
  EXPECT_EQ(unit.container.supported_levels,
            freeisle::core::Bitmask<freeisle::def::Level>{});
  EXPECT_EQ(unit.value, 800);
  EXPECT_EQ(unit.view_range, 4);
  EXPECT_EQ(unit.jamming_range, 1);
}

TEST_F(TestUnitDefHandlers, Save) {
  freeisle::def::UnitDef unit{};
  unit.name = "grunt";
  unit.description = "Some description";
  unit.level = freeisle::def::Level::Land;
  unit.caps = freeisle::def::UnitDef::Cap::Capture;
  unit.armor = 250;
  unit.movement = 500;
  unit.fuel = 35;
  unit.weight = 100;
  unit.movement_cost[freeisle::def::BaseTerrainType::Grass] = 100;
  unit.movement_cost[freeisle::def::BaseTerrainType::Desert] = 150;
  unit.movement_cost[freeisle::def::BaseTerrainType::Snow] = 250;
  unit.movement_cost[freeisle::def::BaseTerrainType::ShallowWater] = 0;
  unit.movement_cost[freeisle::def::BaseTerrainType::DeepWater] = 0;
  unit.movement_cost[freeisle::def::BaseTerrainType::Hill] = 200;
  unit.movement_cost[freeisle::def::BaseTerrainType::Mountain] = 300;
  unit.movement_cost[freeisle::def::OverlayTerrainType::Forest] = 150;
  unit.movement_cost[freeisle::def::OverlayTerrainType::Road] = 90;
  ;
  unit.movement_cost[freeisle::def::OverlayTerrainType::Crevice] = 250;
  unit.movement_cost[freeisle::def::OverlayTerrainType::Fortification] = 100;
  unit.protection[freeisle::def::BaseTerrainType::Grass] = 100;
  unit.protection[freeisle::def::BaseTerrainType::Desert] = 100;
  unit.protection[freeisle::def::BaseTerrainType::Snow] = 100;
  unit.protection[freeisle::def::BaseTerrainType::ShallowWater] = 100;
  unit.protection[freeisle::def::BaseTerrainType::DeepWater] = 100;
  unit.protection[freeisle::def::BaseTerrainType::Hill] = 150;
  unit.protection[freeisle::def::BaseTerrainType::Mountain] = 200;
  unit.protection[freeisle::def::OverlayTerrainType::Forest] = 125;
  unit.protection[freeisle::def::OverlayTerrainType::Road] = 100;
  unit.protection[freeisle::def::OverlayTerrainType::Crevice] = 125;
  unit.protection[freeisle::def::OverlayTerrainType::Fortification] = 200;
  unit.resistance[freeisle::def::DamageType::SmallCaliber] = 100;
  unit.resistance[freeisle::def::DamageType::BigCaliber] = 100;
  unit.resistance[freeisle::def::DamageType::Missile] = 100;
  unit.resistance[freeisle::def::DamageType::Energy] = 100;
  unit.resistance[freeisle::def::DamageType::Explosive] = 80;
  unit.supplies.fuel = 0;
  unit.supplies.repair = 0;
  unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber] = 0;
  unit.supplies.ammo[freeisle::def::DamageType::BigCaliber] = 0;
  unit.supplies.ammo[freeisle::def::DamageType::Missile] = 0;
  unit.supplies.ammo[freeisle::def::DamageType::Energy] = 0;
  unit.supplies.ammo[freeisle::def::DamageType::Explosive] = 0;
  unit.weapons["rifle"] = freeisle::def::WeaponDef{
      .name = "Rifle",
      .damage_type = freeisle::def::DamageType::SmallCaliber,
      .damage = 180,
      .min_range = 1,
      .max_range = 1,
      .ammo = 6};

  unit.container.max_units = 0;
  unit.container.max_weight = 0;
  unit.container.supported_levels = {};
  unit.value = 800;
  unit.view_range = 4;
  unit.jamming_range = 1;

  freeisle::state::serialize::UnitDefSaver saver(aux);
  saver.set(unit);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);

  const std::string expected_value =
      "{\"name\": \"grunt\",\n"
      " \"description\": \"Some description\",\n"
      " \"level\": \"land\",\n"
      " \"caps\": [\"capture\"],\n"
      " \"armor\": 250,\n"
      " \"movement\": 500,\n"
      " \"fuel\": 35,\n"
      " \"weight\": 100,\n"
      " \"movement_cost\": {\n"
      "   \"grass\": 100,\n"
      "   \"desert\": 150,\n"
      "   \"snow\": 250,\n"
      "   \"shallow_water\": 0,\n"
      "   \"deep_water\": 0,\n"
      "   \"hill\": 200,\n"
      "   \"mountain\": 300,\n"
      "   \"forest\": 150,\n"
      "   \"road\": 90,\n"
      "   \"crevice\": 250,\n"
      "   \"fortification\": 100\n"
      " },\n"
      " \"protection\": {\n"
      "   \"grass\": 100,\n"
      "   \"desert\": 100,\n"
      "   \"snow\": 100,\n"
      "   \"shallow_water\": 100,\n"
      "   \"deep_water\": 100,\n"
      "   \"hill\": 150,\n"
      "   \"mountain\": 200,\n"
      "   \"forest\": 125,\n"
      "   \"road\": 100,\n"
      "   \"crevice\": 125,\n"
      "   \"fortification\": 200\n"
      " },\n"
      " \"resistance\": {\n"
      "   \"small_caliber\": 100,\n"
      "   \"big_caliber\": 100,\n"
      "   \"missile\": 100,\n"
      "   \"energy\": 100,\n"
      "   \"explosive\": 80\n"
      " },\n"
      " \"supplies\": {\n"
      "   \"fuel\": 0,\n"
      "   \"repair\": 0,\n"
      "   \"ammo\": {\n"
      "     \"small_caliber\": 0,\n"
      "     \"big_caliber\": 0,\n"
      "     \"missile\": 0,\n"
      "     \"energy\": 0,\n"
      "     \"explosive\": 0\n"
      "   }\n"
      " },\n"
      " \"weapons\": {\n"
      "   \"rifle\": {\n"
      "     \"name\": \"Rifle\",\n"
      "     \"damage_type\": \"small_caliber\",\n"
      "     \"damage\": 180,\n"
      "     \"min_range\": 1,\n"
      "     \"max_range\": 1,\n"
      "     \"ammo\": 6\n"
      "   }\n"
      " },\n"
      " \"container\": {\n"
      "   \"max_units\": 0,\n"
      "   \"max_weight\": 0,\n"
      "   \"supported_levels\": []\n"
      " },\n"
      " \"value\": 800,\n"
      " \"view_range\": 4,\n"
      " \"jamming_range\": 1\n"
      "}\n";
  freeisle::json::test::check(result, expected_value);
}
