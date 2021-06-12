#include "state/serialize/UnitHandlers.hh"

#include "state/Shop.hh"
#include "state/Unit.hh"

#include "core/test/util/Util.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestUnitHandlers : public ::testing::Test {
public:
  TestUnitHandlers()
      : aux{system.logger},
        map_def{.grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)},
        map{
            .def = &map_def,
            .grid = freeisle::core::Grid<freeisle::state::Map::Hex>(5, 5),
        } {
    shop_defs.try_emplace(
        "shop001",
        freeisle::def::ShopDef{
            .container = {.max_units = 2,
                          .max_weight = 2000,
                          .supported_levels = freeisle::def::Level::Land},
            .location = {.x = 3, .y = 1}});
    shop_defs.try_emplace(
        "shop002",
        freeisle::def::ShopDef{
            .container = {.max_units = 2,
                          .max_weight = 2000,
                          .supported_levels = freeisle::def::Level::Land},
            .location = {.x = 4, .y = 3}});
    shop_defs.try_emplace(
        "shop003", freeisle::def::ShopDef{
                       .container = {.max_units = 2,
                                     .max_weight = 2000,
                                     .supported_levels =
                                         {freeisle::def::Level::Water,
                                          freeisle::def::Level::UnderWater}},
                       .location = {.x = 0, .y = 4}});

    unit_defs.try_emplace(
        "def001",
        freeisle::def::UnitDef{
            .name = "def001",
            .level = freeisle::def::Level::Land,
            .movement = 6,
            .fuel = 20,
            .weight = 500,
            .weapons = freeisle::def::make_collection<freeisle::def::WeaponDef>(
                std::make_pair("weapon001",
                               freeisle::def::WeaponDef{.ammo = 4})),
        });

    unit_defs.try_emplace(
        "def001-heavy",
        freeisle::def::UnitDef{
            .name = "def001-heavy",
            .level = freeisle::def::Level::Land,
            .movement = 6,
            .fuel = 20,
            .weight = 1800,
            .weapons = freeisle::def::make_collection<freeisle::def::WeaponDef>(
                std::make_pair("weapon001",
                               freeisle::def::WeaponDef{.ammo = 4})),
        });

    unit_defs.try_emplace("def002", freeisle::def::UnitDef{
                                        .name = "def002",
                                        .level = freeisle::def::Level::Air,
                                        .movement = 10,
                                        .fuel = 45,
                                        .weight = 1500,
                                    });

    unit_defs.try_emplace("def002-air-soar",
                          freeisle::def::UnitDef{
                              .name = "def002",
                              .level = freeisle::def::Level::Air,
                              .caps = freeisle::def::UnitDef::Cap::Soar,
                              .movement = 6,
                              .fuel = 20,
                          });

    unit_defs.try_emplace("def002-air-nosoar",
                          freeisle::def::UnitDef{
                              .name = "def002",
                              .level = freeisle::def::Level::Air,
                              .movement = 6,
                              .fuel = 20,
                          });

    unit_defs.try_emplace("def002-water-soar",
                          freeisle::def::UnitDef{
                              .name = "def002",
                              .level = freeisle::def::Level::Water,
                              .caps = freeisle::def::UnitDef::Cap::Soar,
                              .movement = 6,
                              .fuel = 20,
                          });

    unit_defs.try_emplace("def002-water-nosoar",
                          freeisle::def::UnitDef{
                              .name = "def002",
                              .level = freeisle::def::Level::Water,
                              .movement = 6,
                              .fuel = 20,
                          });

    unit_defs.try_emplace(
        "def003-container",
        freeisle::def::UnitDef{
            .name = "def003",
            .level = freeisle::def::Level::Land,
            .movement = 6,
            .fuel = 20,
            .container = {
                .max_units = 2,
                .max_weight = 2000,
                .supported_levels = freeisle::def::Level::Land,
            }});

    players.try_emplace("player001");
    players.try_emplace("player002");
    players.try_emplace("player003");

    shops.try_emplace("shop_owned", freeisle::state::Shop{
                                        .def = shop_defs.find("shop001"),
                                        .owner = players.find("player001")});
    shops.try_emplace("shop_unowned",
                      freeisle::state::Shop{.def = shop_defs.find("shop002")});
    shops.try_emplace("shop_water", freeisle::state::Shop{
                                        .def = shop_defs.find("shop003"),
                                        .owner = players.find("player001")});
    map.grid(3, 1).shop = shops.find("shop_owned");
    map.grid(4, 3).shop = shops.find("shop_unowned");
    map.grid(0, 4).shop = shops.find("shop_water");

    map_def.grid(0, 3).base_terrain = freeisle::def::BaseTerrainType::DeepWater;
    map_def.grid(0, 4).base_terrain = freeisle::def::BaseTerrainType::DeepWater;
  }

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;

  freeisle::def::MapDef map_def;
  freeisle::state::Map map;
  freeisle::def::Collection<freeisle::def::ShopDef> shop_defs;
  freeisle::def::Collection<freeisle::def::UnitDef> unit_defs;
  freeisle::def::Collection<freeisle::state::Player> players;
  freeisle::def::Collection<freeisle::state::Shop> shops;
};

TEST_F(TestUnitHandlers, LoadUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  const freeisle::state::Unit &unit = units["unit001"];

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def001"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 0);
  EXPECT_EQ(unit.location.y, 0);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 1);
  EXPECT_NE(unit.ammo.find(unit.def->weapons.find("weapon001")),
            unit.ammo.end());
  EXPECT_EQ(unit.ammo.find(unit.def->weapons.find("weapon001"))->second, 3);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  EXPECT_EQ(map.grid(0, 0).surface_unit, units.find("unit001"));
}

TEST_F(TestUnitHandlers, LoadUnitExceedFuelSupply) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 2,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit has more than maximum fuel supply");
    EXPECT_EQ(e.line(), 13);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedRepairSupply) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 10,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit has more than maximum repair supply");
    EXPECT_EQ(e.line(), 14);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedAmmoSupply) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 12,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Unit has more than maximum ammo supply for missile");
    EXPECT_EQ(e.line(), 18);
    EXPECT_EQ(e.col(), 18);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedAmmo) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 5\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Weapon has more than maximum ammo");
    EXPECT_EQ(e.line(), 25);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedHealth) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 120\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit has more than maximum health");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitWrongLevel) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit level does not match unit def");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitAirSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-air-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"high_air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": true,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def002-air-soar"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 0);
  EXPECT_EQ(unit.location.y, 0);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::HighAir);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, true);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 0);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  EXPECT_EQ(map.grid(0, 0).surface_unit, units.find("unit001"));
}

TEST_F(TestUnitHandlers, LoadUnitAirWrongSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-air-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit level does not match unit def");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitAirUnableSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-air-nosoar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"high_air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit level does not match unit def");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitWaterSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"under_water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def002-water-soar"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 0);
  EXPECT_EQ(unit.location.y, 0);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::UnderWater);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 0);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  EXPECT_EQ(map.grid(0, 0).subsurface_unit, units.find("unit001"));
}

TEST_F(TestUnitHandlers, LoadUnitWaterWrongSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit level does not match unit def");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitWaterUnableSoared) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-nosoar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"under_water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit level does not match unit def");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedMovement) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 7,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit has more than maximum movement");
    EXPECT_EQ(e.line(), 7);
    EXPECT_EQ(e.col(), 16);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitExceedFuel) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 0},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 21,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit has more than maximum fuel");
    EXPECT_EQ(e.line(), 8);
    EXPECT_EQ(e.col(), 11);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitUnderwaterOccupiedByShop) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 4},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"under_water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location x=0, y=4 is already occupied");
    EXPECT_EQ(e.line(), 4);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitUnderwaterOccupiedByUnderwaterUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def002-water-soar"),
                                   .location = {.x = 0, .y = 3},
                                   .level = freeisle::def::Level::UnderWater});

  map.grid(0, 3).subsurface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"under_water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location x=0, y=3 is already occupied");
    EXPECT_EQ(e.line(), 4);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitUnderwaterOccupiedByWaterUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def002-water-soar"),
                                   .location = {.x = 0, .y = 3},
                                   .level = freeisle::def::Level::Water});

  map.grid(0, 3).surface_unit = units.find("unit002");
  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"under_water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def002-water-soar"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 0);
  EXPECT_EQ(unit.location.y, 3);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::UnderWater);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 0);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  EXPECT_FALSE(map.grid(0, 3).shop);
  EXPECT_EQ(map.grid(0, 3).surface_unit, units.find("unit002"));
  EXPECT_EQ(map.grid(0, 3).subsurface_unit, units.find("unit001"));
}

TEST_F(TestUnitHandlers, LoadUnitSurfaceOccupiedByShop) { // TODO
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location x=3, y=1 is already occupied");
    EXPECT_EQ(e.line(), 4);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitSurfaceOccupiedByUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def002-water-soar"),
                                   .location = {.x = 0, .y = 3},
                                   .level = freeisle::def::Level::Water});

  map.grid(0, 3).surface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location x=0, y=3 is already occupied");
    EXPECT_EQ(e.line(), 4);
    EXPECT_EQ(e.col(), 15);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitSurfaceOccupiedByUnderWaterUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def002-water-soar"),
                                   .location = {.x = 0, .y = 3},
                                   .level = freeisle::def::Level::UnderWater});

  map.grid(0, 3).subsurface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002-water-soar\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 0, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"water\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def002-water-soar"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 0);
  EXPECT_EQ(unit.location.y, 3);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Water);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 0);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  EXPECT_FALSE(map.grid(0, 3).shop);
  EXPECT_EQ(map.grid(0, 3).surface_unit, units.find("unit001"));
  EXPECT_EQ(map.grid(0, 3).subsurface_unit, units.find("unit002"));
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShop) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def001"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 3);
  EXPECT_EQ(unit.location.y, 1);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 1);
  EXPECT_NE(unit.ammo.find(unit.def->weapons.find("weapon001")),
            unit.ammo.end());
  EXPECT_EQ(unit.ammo.find(unit.def->weapons.find("weapon001"))->second, 3);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_EQ(unit.contained_in_shop, shops.find("shop_owned"));
  EXPECT_FALSE(unit.contained_in_unit);

  freeisle::def::Ref<freeisle::state::Shop> shop = shops.find("shop_owned");
  EXPECT_EQ(shop->container.units.size(), 1);
  EXPECT_EQ(shop->container.units.front(), units.find("unit001"));

  EXPECT_FALSE(map.grid(3, 1).surface_unit);
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopWrongOwner) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 4, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_unowned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained unit \"unit001\" has different owner than container");
    EXPECT_EQ(e.line(), 1); // TODO(armin): make this point to owner?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopWrongLocation) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Contained unit \"unit001\" location does not match "
                           "container location");
    EXPECT_EQ(
        e.line(),
        1); // TODO(armin): make this point to contained_in_shop, or location?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopExceedCount) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace(
      "unit002",
      freeisle::state::Unit{.def = unit_defs.find("def001"),
                            .owner = players.find("player001"),
                            .contained_in_shop = shops.find("shop_owned")});
  units.try_emplace(
      "unit003",
      freeisle::state::Unit{.def = unit_defs.find("def001"),
                            .owner = players.find("player002"),
                            .contained_in_shop = shops.find("shop_owned")});
  shops.find("shop_owned")
      ->second.container.units.push_back(units.find("unit002"));
  shops.find("shop_owned")
      ->second.container.units.push_back(units.find("unit003"));

  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Container has more units contained than allowed by def");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_shop?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopExceedWeight) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace(
      "unit002",
      freeisle::state::Unit{.def = unit_defs.find("def001"),
                            .owner = players.find("player001"),
                            .location = {.x = 3, .y = 1},
                            .level = freeisle::def::Level::Land,
                            .contained_in_shop = shops.find("shop_owned")});
  shops.find("shop_owned")
      ->second.container.units.push_back(units.find("unit002"));

  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001-heavy\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained units exceed maximum weight allowed by container def");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_shop?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopWrongLevel) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit001\" has level not supported by container");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_shop?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInLoadedUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });
  map.grid(2, 2).surface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def001"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 2);
  EXPECT_EQ(unit.location.y, 2);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 1);
  EXPECT_NE(unit.ammo.find(unit.def->weapons.find("weapon001")),
            unit.ammo.end());
  EXPECT_EQ(unit.ammo.find(unit.def->weapons.find("weapon001"))->second, 3);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_EQ(unit.contained_in_unit, units.find("unit002"));

  freeisle::def::Ref<freeisle::state::Unit> container = units.find("unit002");
  EXPECT_EQ(container->container.units.size(), 1);
  EXPECT_EQ(container->container.units.front(), units.find("unit001"));

  EXPECT_EQ(map.grid(2, 2).surface_unit, container);
}

TEST_F(TestUnitHandlers, LoadUnitContainedInUnloadedUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit001"];
  units.try_emplace("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def001"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 3);
  EXPECT_EQ(unit.location.y, 1);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 1);
  EXPECT_NE(unit.ammo.find(unit.def->weapons.find("weapon001")),
            unit.ammo.end());
  EXPECT_EQ(unit.ammo.find(unit.def->weapons.find("weapon001"))->second, 3);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);

  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_EQ(unit.contained_in_unit, units.find("unit002"));

  freeisle::def::Ref<freeisle::state::Unit> container = units.find("unit002");
  EXPECT_EQ(container->container.units.size(), 1);
  EXPECT_EQ(container->container.units.front(), units.find("unit001"));

  EXPECT_FALSE(map.grid(2, 2).surface_unit);
}

TEST_F(TestUnitHandlers, LoadUnitContainer) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 4, .y = 4},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002"),
                               });
  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 4, \"y\": 4},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(unit.def);
  ASSERT_TRUE(unit.owner);

  EXPECT_EQ(unit.def, unit_defs.find("def003-container"));
  EXPECT_EQ(unit.owner, players.find("player001"));
  EXPECT_EQ(unit.location.x, 4);
  EXPECT_EQ(unit.location.y, 4);
  EXPECT_EQ(unit.health, 100);
  EXPECT_EQ(unit.level, freeisle::def::Level::Land);
  EXPECT_EQ(unit.movement, 6);
  EXPECT_EQ(unit.fuel, 20);
  EXPECT_EQ(unit.experience, 2);
  EXPECT_EQ(unit.has_actioned, true);
  EXPECT_EQ(unit.has_soared, false);
  EXPECT_EQ(unit.supplies.fuel, 0);
  EXPECT_EQ(unit.supplies.repair, 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::SmallCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::BigCaliber], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Missile], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Energy], 0);
  EXPECT_EQ(unit.supplies.ammo[freeisle::def::DamageType::Explosive], 0);
  EXPECT_EQ(unit.ammo.size(), 0);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_EQ(unit.container.units.size(), 1);
  EXPECT_EQ(unit.container.units.front(), units.find("unit001"));

  EXPECT_EQ(map.grid(4, 4).surface_unit, units.find("unit002"));
}

TEST_F(TestUnitHandlers, LoadUnitContainedInLoadedUnitWrongOwner) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });
  map.grid(2, 2).surface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player002\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained unit \"unit001\" has different owner than container");
    EXPECT_EQ(e.line(), 1); // TODO(armin): make this point to owner?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInLoadedUnitWrongLocation) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });
  map.grid(2, 2).surface_unit = units.find("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Contained unit \"unit001\" location does not match "
                           "container location");
    EXPECT_EQ(
        e.line(),
        1); // TODO(armin): make this point to location or contained_in_unit
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInUnitExceedCount) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });
  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .contained_in_unit = units.find("unit002")});
  units.try_emplace("unit004", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .contained_in_unit = units.find("unit002")});
  units.find("unit002")->second.container.units.push_back(
      units.find("unit003"));
  units.find("unit002")->second.container.units.push_back(
      units.find("unit004"));

  map.grid(2, 2).surface_unit = units.find("unit002");
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Container has more units contained than allowed by def");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_shop?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInLoadedUnitExceedWeight) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });

  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  units.find("unit002")->second.container.units.push_back(
      units.find("unit003"));

  map.grid(2, 2).surface_unit = units.find("unit002");
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001-heavy\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained units exceed maximum weight allowed by container def");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_unit?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInLoadedUnitWrongLevel) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                               });
  map.grid(2, 2).surface_unit = units.find("unit002");

  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def002\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"air\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit001\" has level not supported by container");
    EXPECT_EQ(e.line(),
              1); // TODO(armin): make this point to contained_in_shop?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerWrongOwner) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player002\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained unit \"unit001\" has different owner than container");
    EXPECT_EQ(e.line(), 1); // TODO(armin): make this point to owner?
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerWrongLocation) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Contained unit \"unit001\" location does not match "
                           "container location");
    EXPECT_EQ(e.line(), 1); // TODO(armin): make this point to location
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerExceedCount) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];

  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  units.try_emplace("unit004", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  unit.container.units.push_back(units.find("unit001"));
  unit.container.units.push_back(units.find("unit003"));
  unit.container.units.push_back(units.find("unit004"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Container has more units contained than allowed by def");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerExceedWeight) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];

  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def001-heavy"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});
  unit.container.units.push_back(units.find("unit001"));
  unit.container.units.push_back(units.find("unit003"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Contained units exceed maximum weight allowed by container def");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerWrongLevel) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 2, .y = 2},
                                   .level = freeisle::def::Level::Air,
                                   .contained_in_unit = units.find("unit002")});
  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 2, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  }\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit001\" has level not supported by container");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInShopAndUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace("unit002");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded =
      "{\n"
      "  \"def\": \"def001\",\n"
      "  \"owner\": \"player001\",\n"
      "  \"location\": {\"x\": 4, \"y\": 3},\n"
      "  \"health\": 100\n,"
      "  \"level\": \"land\"\n,"
      "  \"movement\": 6,\n"
      "  \"fuel\": 20,\n"
      "  \"experience\": 2,\n"
      "  \"has_actioned\": true,\n"
      "  \"has_soared\": false,\n"
      "  \"supplies\": {\n"
      "    \"fuel\": 0,\n"
      "    \"repair\": 0,\n"
      "    \"ammo\": {\n"
      "      \"small_caliber\": 0,\n"
      "      \"big_caliber\": 0,\n"
      "      \"missile\": 0,\n"
      "      \"energy\": 0,\n"
      "      \"explosive\": 0\n"
      "    }\n"
      "  },\n"
      "  \"weapons\": {\n"
      "    \"weapon001\": {\n"
      "      \"ammo\": 3\n"
      "    }\n"
      "  },\n"
      "  \"stats\": {\n"
      "    \"hits_dealt\": 1,\n"
      "    \"hits_taken\": 2,\n"
      "    \"damage_dealt\": 3,\n"
      "    \"damage_taken\": 4,\n"
      "    \"hexes_moved\": 5\n"
      "  },\n"
      "  \"contained_in_shop\": \"shop_unowned\",\n"
      "  \"contained_in_unit\": \"unit002\"\n"
      "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit cannot be contained in both unit and shop");
    EXPECT_EQ(e.line(), 35);
    EXPECT_EQ(e.col(), 24);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInItself) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 4, \"y\": 3},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit cannot be contained in itself");
    EXPECT_EQ(e.line(), 35);
    EXPECT_EQ(e.col(), 24);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInUnitContainedInShop) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001");
  units.try_emplace(
      "unit002",
      freeisle::state::Unit{.def = unit_defs.find("def003-container"),
                            .owner = players.find("player001"),
                            .location = {.x = 3, .y = 1},
                            .level = freeisle::def::Level::Land,
                            .contained_in_shop = shops.find("shop_owned")});
  shops.find("shop_owned")
      ->second.container.units.push_back(units.find("unit002"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit002\" has other unit \"unit001\" contained");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerInShop) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("unit001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 3, .y = 1},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit002")});

  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 1},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_shop\": \"shop_owned\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit002\" has other unit \"unit001\" contained");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainedInUnitContainedInUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 3, .y = 2},
                                   .level = freeisle::def::Level::Land});
  units.try_emplace("unit002", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 3, .y = 2},
                                   .level = freeisle::def::Level::Land,
                                   .contained_in_unit = units.find("unit003")});
  units.try_emplace("unit001");
  units.find("unit003")->second.container.units.push_back(
      units.find("unit002"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def001\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {\n"
                                   "    \"weapon001\": {\n"
                                   "      \"ammo\": 3\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit002\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit002\" has other unit \"unit001\" contained");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, LoadUnitContainerInUnit) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit003", freeisle::state::Unit{
                                   .def = unit_defs.find("def003-container"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 3, .y = 2}});
  freeisle::state::Unit &unit = units["unit002"];
  units.try_emplace("unit001", freeisle::state::Unit{
                                   .def = unit_defs.find("def001"),
                                   .owner = players.find("player001"),
                                   .location = {.x = 3, .y = 1},
                                   .contained_in_unit = units.find("unit002")});

  unit.container.units.push_back(units.find("unit001"));

  freeisle::state::serialize::UnitLoader loader(unit_defs, map, shops, units,
                                                players);
  loader.set(units.find("unit002"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"def003-container\",\n"
                                   "  \"owner\": \"player001\",\n"
                                   "  \"location\": {\"x\": 3, \"y\": 2},\n"
                                   "  \"health\": 100\n,"
                                   "  \"level\": \"land\"\n,"
                                   "  \"movement\": 6,\n"
                                   "  \"fuel\": 20,\n"
                                   "  \"experience\": 2,\n"
                                   "  \"has_actioned\": true,\n"
                                   "  \"has_soared\": false,\n"
                                   "  \"supplies\": {\n"
                                   "    \"fuel\": 0,\n"
                                   "    \"repair\": 0,\n"
                                   "    \"ammo\": {\n"
                                   "      \"small_caliber\": 0,\n"
                                   "      \"big_caliber\": 0,\n"
                                   "      \"missile\": 0,\n"
                                   "      \"energy\": 0,\n"
                                   "      \"explosive\": 0\n"
                                   "    }\n"
                                   "  },\n"
                                   "  \"weapons\": {},\n"
                                   "  \"stats\": {\n"
                                   "    \"hits_dealt\": 1,\n"
                                   "    \"hits_taken\": 2,\n"
                                   "    \"damage_dealt\": 3,\n"
                                   "    \"damage_taken\": 4,\n"
                                   "    \"hexes_moved\": 5\n"
                                   "  },\n"
                                   "  \"contained_in_unit\": \"unit003\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(
        e.message(),
        "Contained unit \"unit002\" has other unit \"unit001\" contained");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestUnitHandlers, Save) {
  freeisle::def::Collection<freeisle::state::Unit> units;

  units.try_emplace(
      "unit001",
      freeisle::state::Unit{
          .def = unit_defs.find("def001"),
          .owner = players.find("player001"),
          .location = {.x = 2, .y = 3},
          .health = 80,
          .level = freeisle::def::Level::Land,
          .movement = 4,
          .fuel = 18,
          .experience = 1,
          .has_actioned = false,
          .has_soared = false,
          .supplies = {.fuel = 0, .repair = 0},
          .ammo = freeisle::def::make_ref_map<freeisle::def::WeaponDef,
                                              uint32_t>(std::make_pair(
              unit_defs.find("def001")->second.weapons.find("weapon001"), 2)),
          .stats = {.hits_dealt = 0,
                    .hits_taken = 2,
                    .damage_dealt = 10,
                    .damage_taken = 20,
                    .hexes_moved = 3}});

  freeisle::state::serialize::UnitSaver saver(unit_defs, shops, units, players);
  saver.set(units.find("unit001"));

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);
  const std::string expected = "{\n"
                               "  \"def\": \"def001\",\n"
                               "  \"owner\": \"player001\",\n"
                               "  \"location\": {\"x\": 2, \"y\": 3},\n"
                               "  \"health\": 80,\n"
                               "  \"level\": \"land\"\n,"
                               "  \"movement\": 4,\n"
                               "  \"fuel\": 18,\n"
                               "  \"experience\": 1,\n"
                               "  \"has_actioned\": false,\n"
                               "  \"has_soared\": false,\n"
                               "  \"supplies\": {\n"
                               "    \"fuel\": 0,\n"
                               "    \"repair\": 0,\n"
                               "    \"ammo\": {\n"
                               "      \"small_caliber\": 0,\n"
                               "      \"big_caliber\": 0,\n"
                               "      \"missile\": 0,\n"
                               "      \"energy\": 0,\n"
                               "      \"explosive\": 0\n"
                               "    }\n"
                               "  },\n"
                               "  \"weapons\": {\n"
                               "    \"weapon001\": {\n"
                               "      \"ammo\": 2\n"
                               "    }\n"
                               "  },\n"
                               "  \"stats\": {\n"
                               "    \"hits_dealt\": 0,\n"
                               "    \"hits_taken\": 2,\n"
                               "    \"damage_dealt\": 10,\n"
                               "    \"damage_taken\": 20,\n"
                               "    \"hexes_moved\": 3\n"
                               "  }\n"
                               "}\n";

  freeisle::json::test::check(result, expected);
}
