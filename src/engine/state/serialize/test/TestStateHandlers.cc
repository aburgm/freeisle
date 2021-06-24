#include "state/serialize/StateHandlers.hh"

#include "def/Scenario.hh"
#include "state/State.hh"

#include "core/test/util/Util.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestStateHandlers : public ::testing::Test {
public:
  TestStateHandlers()
      : aux{system.logger},
        scenario{.name = "My Scenario",
                 .description = "My description",
                 .map{.grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(
                          5, 5)}} {
    scenario.shops.try_emplace(
        "shop001",
        freeisle::def::ShopDef{
            .container = {.max_units = 2,
                          .max_weight = 2000,
                          .supported_levels = freeisle::def::Level::Land},
            .location = {.x = 3, .y = 1}});

    scenario.units.try_emplace(
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
  }

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;
  freeisle::def::Scenario scenario;
};

TEST_F(TestStateHandlers, LoadState) {
  freeisle::state::State state;
  freeisle::state::serialize::StateLoader loader(state, scenario, aux);

  const std::string to_be_loaded =
      "{\n"
      "  \"teams\": {\n"
      "    \"north\": {\n"
      "      \"name\": \"North\"\n"
      "    },\n"
      "    \"south\": {\n"
      "      \"name\": \"South\"\n"
      "    }\n"
      "  },\n"
      "  \"players\": {\n"
      "    \"player001\": {\n"
      "      \"name\": \"North Player\",\n"
      "      \"color\": {\n"
      "        \"red\": 0,\n"
      "        \"green\": 0,\n"
      "        \"blue\": 0\n"
      "      },\n"
      "      \"team\": \"north\"\n,"
      "      \"fow\": \"AAAAAA==\",\n"
      "      \"wealth\": 100,\n"
      "      \"captain\": \"unit001\",\n"
      "      \"lose_conditions\": [\"eliminate_captain\"],\n"
      "      \"is_eliminated\": false\n"
      "    }\n"
      "  },\n"
      "  \"shops\": {\n"
      "    \"shop001\": {\n"
      "      \"def\": \"shop001\",\n"
      "      \"owner\": \"player001\""
      "    }\n"
      "  },\n"
      "  \"units\": {\n"
      "    \"unit001\": {\n"
      "      \"def\": \"def001\",\n"
      "      \"owner\": \"player001\",\n"
      "      \"location\": {\"x\": 0, \"y\": 0},\n"
      "      \"health\": 100\n,"
      "      \"level\": \"land\",\n"
      "      \"movement\": 6,\n"
      "      \"fuel\": 20,\n"
      "      \"experience\": 2,\n"
      "      \"has_actioned\": true,\n"
      "      \"has_soared\": false,\n"
      "      \"supplies\": {\n"
      "        \"fuel\": 0,\n"
      "        \"repair\": 0,\n"
      "        \"ammo\": {\n"
      "          \"small_caliber\": 0,\n"
      "          \"big_caliber\": 0,\n"
      "          \"missile\": 0,\n"
      "          \"energy\": 0,\n"
      "          \"explosive\": 0\n"
      "        }\n"
      "      },\n"
      "      \"weapons\": {\n"
      "        \"weapon001\": {\n"
      "          \"ammo\": 2\n"
      "        }\n"
      "      },\n"
      "      \"stats\": {\n"
      "        \"hits_dealt\": 1,\n"
      "        \"hits_taken\": 2,\n"
      "        \"damage_dealt\": 3,\n"
      "        \"damage_taken\": 4,\n"
      "        \"hexes_moved\": 5\n"
      "      }\n"
      "    }\n"
      "  },\n"
      "  \"turn\": 1,\n"
      "  \"player_at_turn\": \"player001\"\n"
      "}\n";

  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  EXPECT_EQ(state.scenario, &scenario);
  EXPECT_EQ(state.teams.size(), 2);
  EXPECT_EQ(state.teams.count("north"), 1);
  EXPECT_EQ(state.teams.count("south"), 1);
  EXPECT_EQ(state.teams["north"].name, "North");
  EXPECT_EQ(state.teams["south"].name, "South");
  EXPECT_EQ(state.players.size(), 1);
  EXPECT_EQ(state.players.count("player001"), 1);
  EXPECT_EQ(state.players["player001"].name, "North Player");
  EXPECT_EQ(state.players["player001"].team, state.teams.find("north"));
  EXPECT_EQ(state.players["player001"].wealth, 100);
  EXPECT_EQ(state.players["player001"].captain, state.units.find("unit001"));
  EXPECT_TRUE(state.players["player001"].lose_conditions.is_set(
      freeisle::def::Goal::EliminateCaptain));
  EXPECT_EQ(state.players["player001"].is_eliminated, false);
  EXPECT_EQ(state.shops.size(), 1);
  EXPECT_EQ(state.shops.count("shop001"), 1);
  EXPECT_EQ(state.shops["shop001"].def, scenario.shops.find("shop001"));
  EXPECT_EQ(state.shops["shop001"].owner, state.players.find("player001"));
  EXPECT_EQ(state.units.size(), 1);
  EXPECT_EQ(state.units.count("unit001"), 1);

  freeisle::state::Unit &unit = state.units["unit001"];
  EXPECT_EQ(unit.def, scenario.units.find("def001"));
  EXPECT_EQ(unit.owner, state.players.find("player001"));
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
  EXPECT_EQ(unit.ammo.find(unit.def->weapons.find("weapon001"))->second, 2);
  EXPECT_EQ(unit.stats.hits_dealt, 1);
  EXPECT_EQ(unit.stats.hits_taken, 2);
  EXPECT_EQ(unit.stats.damage_dealt, 3);
  EXPECT_EQ(unit.stats.damage_taken, 4);
  EXPECT_EQ(unit.stats.hexes_moved, 5);
  EXPECT_FALSE(unit.contained_in_shop);
  EXPECT_FALSE(unit.contained_in_unit);

  ASSERT_EQ(state.map.grid.width(), 5);
  ASSERT_EQ(state.map.grid.height(), 5);

  EXPECT_EQ(state.map.grid(0, 0).surface_unit, state.units.find("unit001"));
  EXPECT_EQ(state.map.grid(3, 1).shop, state.shops.find("shop001"));
}

TEST_F(TestStateHandlers, Save) {
  freeisle::state::State state{.scenario = &scenario};
  state.teams.try_emplace("north", freeisle::state::Team{.name = "North"});
  state.teams.try_emplace("south", freeisle::state::Team{.name = "South"});
  state.players.try_emplace(
      "player001",
      freeisle::state::Player{
          .name = "North Player",
          .color = {.r = 0, .g = 0, .b = 0},
          .team = state.teams.find("north"),
          .fow = freeisle::core::Grid<freeisle::state::Player::Fow>(5, 5),
          .wealth = 100,
          .lose_conditions = freeisle::def::Goal::EliminateCaptain,
          .is_eliminated = false});

  state.shops.try_emplace(
      "shop001",
      freeisle::state::Shop{.def = scenario.shops.find("shop001"),
                            .owner = state.players.find("player001")});

  state.units.try_emplace(
      "unit001",
      freeisle::state::Unit{
          .def = scenario.units.find("def001"),
          .owner = state.players.find("player001"),
          .location = {.x = 0, .y = 0},
          .health = 100,
          .level = freeisle::def::Level::Land,
          .movement = 6,
          .fuel = 20,
          .experience = 2,
          .has_actioned = true,
          .has_soared = false,
          .supplies = {.fuel = 0, .repair = 0},
          .ammo = freeisle::def::make_ref_map<freeisle::def::WeaponDef,
                                              uint32_t>(std::make_pair(
              scenario.units.find("def001")->second.weapons.find("weapon001"),
              2)),
          .stats = {.hits_dealt = 1,
                    .hits_taken = 2,
                    .damage_dealt = 3,
                    .damage_taken = 4,
                    .hexes_moved = 5}});

  state.players["player001"].captain = state.units.find("unit001");

  state.map.grid = freeisle::core::Grid<freeisle::state::Map::Hex>(5, 5);
  state.map.grid(0, 0).surface_unit = state.units.find("unit001");
  state.map.grid(3, 1).shop = state.shops.find("shop001");

  state.turn_num = 1;
  state.player_at_turn = state.players.find("player001");

  freeisle::state::serialize::StateSaver saver(state, aux);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);
  const std::string expected =
      "{\n"
      "  \"teams\": {\n"
      "    \"north\": {\n"
      "      \"name\": \"North\"\n"
      "    },\n"
      "    \"south\": {\n"
      "      \"name\": \"South\"\n"
      "    }\n"
      "  },\n"
      "  \"players\": {\n"
      "    \"player001\": {\n"
      "      \"name\": \"North Player\",\n"
      "      \"color\": {\n"
      "        \"red\": 0,\n"
      "        \"green\": 0,\n"
      "        \"blue\": 0\n"
      "      },\n"
      "      \"team\": \"north\"\n,"
      "      \"fow\": \"AAAAAA==\",\n"
      "      \"wealth\": 100,\n"
      "      \"captain\": \"unit001\",\n"
      "      \"lose_conditions\": [\"eliminate_captain\"],\n"
      "      \"is_eliminated\": false\n"
      "    }\n"
      "  },\n"
      "  \"shops\": {\n"
      "    \"shop001\": {\n"
      "      \"def\": \"shop001\",\n"
      "      \"owner\": \"player001\""
      "    }\n"
      "  },\n"
      "  \"units\": {\n"
      "    \"unit001\": {\n"
      "      \"def\": \"def001\",\n"
      "      \"owner\": \"player001\",\n"
      "      \"location\": {\"x\": 0, \"y\": 0},\n"
      "      \"health\": 100\n,"
      "      \"level\": \"land\",\n"
      "      \"movement\": 6,\n"
      "      \"fuel\": 20,\n"
      "      \"experience\": 2,\n"
      "      \"has_actioned\": true,\n"
      "      \"has_soared\": false,\n"
      "      \"supplies\": {\n"
      "        \"fuel\": 0,\n"
      "        \"repair\": 0,\n"
      "        \"ammo\": {\n"
      "          \"small_caliber\": 0,\n"
      "          \"big_caliber\": 0,\n"
      "          \"missile\": 0,\n"
      "          \"energy\": 0,\n"
      "          \"explosive\": 0\n"
      "        }\n"
      "      },\n"
      "      \"weapons\": {\n"
      "        \"weapon001\": {\n"
      "          \"ammo\": 2\n"
      "        }\n"
      "      },\n"
      "      \"stats\": {\n"
      "        \"hits_dealt\": 1,\n"
      "        \"hits_taken\": 2,\n"
      "        \"damage_dealt\": 3,\n"
      "        \"damage_taken\": 4,\n"
      "        \"hexes_moved\": 5\n"
      "      }\n"
      "    }\n"
      "  },\n"
      "  \"turn\": 1,\n"
      "  \"player_at_turn\": \"player001\"\n"
      "}\n";

  freeisle::json::test::check(result, expected);
}
