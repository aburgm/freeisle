#include "def/serialize/UnitDefHandlers.hh"

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
  freeisle::def::serialize::AuxData aux;
};

TEST_F(TestUnitDefHandlers, Load) {
  freeisle::def::Collection<freeisle::def::UnitDef> units;
  freeisle::def::UnitDef &unit = units["unit001"];
  freeisle::def::serialize::UnitDefLoader loader(aux);
  loader.set(units.find("unit001"));
  freeisle::json::loader::load_root_object("data/unit_grunt.json", loader);

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

  const freeisle::def::Collection<freeisle::def::UnitDef> units = {
      {"unit001", unit}};

  freeisle::def::serialize::UnitDefSaver saver(aux);
  saver.set(units.find("unit001"));

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);
  const std::vector<uint8_t> expected =
      freeisle::fs::read_file("data/unit_grunt.json", nullptr);

  freeisle::json::test::check(result, expected);
}
