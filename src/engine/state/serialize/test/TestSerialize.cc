#include "state/serialize/Serialize.hh"

#include "fs/File.hh"
#include "fs/Path.hh"
#include "png/Png.hh"

#include "core/test/util/Util.hh"
#include "fs/test/util/TempDirFixture.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestSerialize : public ::freeisle::fs::test::TempDirFixture {
public:
  TestSerialize() {}

  freeisle::log::test::System system;
};

TEST_F(TestSerialize, CreateScenarioWithoutPlayers) {
  const std::string base_dir = freeisle::fs::path::dirname(
      freeisle::fs::path::dirname(freeisle::fs::path::dirname(orig_directory)));

  const freeisle::state::serialize::CreateOptions options = {
      .name = "My Scenario",
      .description = "East End Boys and West End Girls",
      .width = 10,
      .height = 10,
      .players = {},
      .base_dir = base_dir,
      .unit_defs = {"def/serialize/test/data/unit_grunt.json"},
      .decoration_defs = {"state/serialize/test/data/deco_flowers.json"}};

  EXPECT_THROW(freeisle::state::serialize::create_scenario(
                   options, system.logger.make_child_logger("test")),
               std::invalid_argument);
}

TEST_F(TestSerialize, CreateAndSaveScenario) {
  const std::string base_dir = freeisle::fs::path::dirname(
      freeisle::fs::path::dirname(freeisle::fs::path::dirname(orig_directory)));

  const freeisle::state::serialize::CreateOptions options = {
      .name = "My Scenario",
      .description = "East End Boys and West End Girls",
      .width = 10,
      .height = 10,
      .players = {{"my_player", {255, 0, 0}}},
      .base_dir = base_dir,
      .unit_defs = {"def/serialize/test/data/unit_grunt.json"},
      .decoration_defs = {"state/serialize/test/data/deco_flowers.json"}};

  freeisle::state::serialize::SerializableState state =
      freeisle::state::serialize::create_scenario(
          options, system.logger.make_child_logger("test"));
  ASSERT_NE(state.scenario, nullptr);

  const freeisle::def::Scenario &scenario = *state.scenario;
  const freeisle::state::State &game_state = state.state;

  EXPECT_EQ(scenario.name, "My Scenario");
  EXPECT_EQ(scenario.description, "East End Boys and West End Girls");
  ASSERT_EQ(scenario.map.grid.width(), 10);
  ASSERT_EQ(scenario.map.grid.height(), 10);
  for (uint32_t y = 0; y < 10; ++y) {
    for (uint32_t x = 0; x < 10; ++x) {
      EXPECT_EQ(scenario.map.grid(x, y).base_terrain,
                freeisle::def::BaseTerrainType::Grass);
      EXPECT_FALSE(scenario.map.grid(x, y).overlay_terrain);
      EXPECT_EQ(scenario.map.grid(x, y).decoration, nullptr);
    }
  }
  EXPECT_EQ(scenario.map.decoration_defs.size(), 1);
  ASSERT_EQ(scenario.map.decoration_defs.count("deco001"), 1);
  EXPECT_EQ(scenario.map.decoration_defs.find("deco001")->second.name,
            "flowers");
  EXPECT_EQ(scenario.units.size(), 1);
  ASSERT_EQ(scenario.units.count("unitdef001"), 1);
  EXPECT_EQ(scenario.units.find("unitdef001")->second.name, "grunt");

  EXPECT_EQ(game_state.scenario, &scenario);
  EXPECT_EQ(game_state.teams.size(), 0);
  ASSERT_EQ(game_state.players.size(), 1);
  ASSERT_EQ(game_state.players.count("player001"), 1);
  EXPECT_EQ(game_state.map.grid.width(), 10);
  EXPECT_EQ(game_state.map.grid.height(), 10);
  EXPECT_EQ(game_state.shops.size(), 0);
  EXPECT_EQ(game_state.units.size(), 0);
  EXPECT_EQ(game_state.turn_num, 1);
  EXPECT_EQ(game_state.player_at_turn, game_state.players.begin());

  const freeisle::def::Collection<freeisle::state::Player>::const_iterator
      iter = game_state.players.find("player001");
  ASSERT_NE(iter, game_state.players.end());
  const freeisle::state::Player &player = iter->second;

  EXPECT_EQ(player.name, "my_player");
  EXPECT_EQ(player.color.r, 255);
  EXPECT_EQ(player.color.g, 0);
  EXPECT_EQ(player.color.b, 0);
  EXPECT_FALSE(player.team);
  EXPECT_EQ(player.fow.width(), 10);
  EXPECT_EQ(player.fow.height(), 10);
  for (uint32_t y = 0; y < 10; ++y) {
    for (uint32_t x = 0; x < 10; ++x) {
      EXPECT_EQ(player.fow(x, y).discovered, false);
      EXPECT_EQ(player.fow(x, y).view, 0);
    }
  }
  EXPECT_EQ(player.wealth, 0);
  EXPECT_FALSE(player.captain);
  EXPECT_EQ(player.lose_conditions,
            freeisle::core::Bitmask<freeisle::def::Goal>{});
  EXPECT_EQ(player.is_eliminated, false);
  EXPECT_EQ(player.units.size(), 0);

  // Test save with includes
  freeisle::state::serialize::save(state, "save1.json",
                                   system.logger.make_child_logger("test"));
  const std::vector<uint8_t> result =
      freeisle::fs::read_file("save1.json", nullptr);

  const std::string expected =
      "{\n"
      "  \"scenario\": {\n"
      "    \"name\": \"My Scenario\",\n"
      "    \"description\": \"East End Boys and West End Girls\",\n"
      "    \"shops\": {},\n"
      "    \"units\": {\n"
      "      \"unitdef001\": {\n"
      "        \"include\": \"def/serialize/test/data/unit_grunt.json\"\n"
      "      }\n"
      "    },\n"
      "    \"map\": {\n"
      "      \"grid\": \"file:map.png\",\n"
      "      \"decorations\": {\n"
      "        \"deco001\": {\n"
      "          \"include\": \"state/serialize/test/data/deco_flowers.json\"\n"
      "        }\n"
      "      }\n"
      "    }\n"
      "  },\n"
      "  \"teams\": {},\n"
      "  \"players\": {"
      "    \"player001\": {\n"
      "      \"name\": \"my_player\",\n"
      "      \"color\": {\n"
      "        \"red\": 255,\n"
      "        \"green\": 0,\n"
      "        \"blue\": 0\n"
      "      },\n"
      "      \"fow\": \"file:fow.1.bin\",\n"
      "      \"is_eliminated\": false,\n"
      "      \"lose_conditions\": [],\n"
      "      \"wealth\": 0\n"
      "    }\n"
      "  },\n"
      "  \"shops\": {},\n"
      "  \"units\": {},\n"
      "  \"turn\": 1\n,"
      "  \"player_at_turn\": \"player001\"\n"
      "}\n";

  freeisle::json::test::check(result, expected);

  const std::vector<uint8_t> mapPng =
      freeisle::fs::read_file("map.png", nullptr);
  const freeisle::core::Grid<freeisle::core::color::Rgb8> map =
      freeisle::png::decode_rgb8(mapPng.data(), mapPng.size(),
                                 system.logger.make_child_logger("test"));

  EXPECT_EQ(map.width(), 10);
  EXPECT_EQ(map.height(), 10);
  for (uint32_t y = 0; y < 10; ++y) {
    for (uint32_t x = 0; x < 10; ++x) {
      EXPECT_EQ(map(x, y).r, 0);
      EXPECT_EQ(map(x, y).g, 0);
      EXPECT_EQ(map(x, y).b, 0);
    }
  }

  // Test save standalone
  freeisle::state::serialize::save(game_state, "save2.json",
                                   system.logger.make_child_logger("test"));
  const std::vector<uint8_t> standaloneResult =
      freeisle::fs::read_file("save2.json", nullptr);

  const std::string standaloneExpected =
      "{\n"
      "  \"scenario\": {\n"
      "    \"name\": \"My Scenario\",\n"
      "    \"description\": \"East End Boys and West End Girls\",\n"
      "    \"shops\": {},\n"
      "    \"units\": {\n"
      "      \"unitdef001\": {\n"
      "        \"name\": \"grunt\",\n"
      "        \"description\": \"Some description\",\n"
      "        \"level\": \"land\",\n"
      "        \"caps\": [\"capture\"],\n"
      "        \"armor\": 250,\n"
      "        \"movement\": 500,\n"
      "        \"fuel\": 35,\n"
      "        \"weight\": 100,\n"
      "        \"movement_cost\": {\n"
      "          \"grass\": 100,\n"
      "          \"desert\": 150,\n"
      "          \"snow\": 250,\n"
      "          \"shallow_water\": 0,\n"
      "          \"deep_water\": 0,\n"
      "          \"hill\": 200,\n"
      "          \"mountain\": 300,\n"
      "          \"forest\": 150,\n"
      "          \"road\": 90,\n"
      "          \"crevice\": 250,\n"
      "          \"fortification\": 100\n"
      "        },\n"
      "        \"protection\": {\n"
      "          \"grass\": 100,\n"
      "          \"desert\": 100,\n"
      "          \"snow\": 100,\n"
      "          \"shallow_water\": 100,\n"
      "          \"deep_water\": 100,\n"
      "          \"hill\": 150,\n"
      "          \"mountain\": 200,\n"
      "          \"forest\": 125,\n"
      "          \"road\": 100,\n"
      "          \"crevice\": 125,\n"
      "          \"fortification\": 200\n"
      "        },\n"
      "        \"resistance\": {\n"
      "          \"small_caliber\": 100,\n"
      "          \"big_caliber\": 100,\n"
      "          \"missile\": 100,\n"
      "          \"energy\": 100,\n"
      "          \"explosive\": 80\n"
      "        },\n"
      "        \"supplies\": {\n"
      "          \"fuel\": 0,\n"
      "          \"repair\": 0,\n"
      "          \"ammo\": {\n"
      "            \"small_caliber\": 0,\n"
      "            \"big_caliber\": 0,\n"
      "            \"missile\": 0,\n"
      "            \"energy\": 0,\n"
      "            \"explosive\": 0\n"
      "          }\n"
      "        },\n"
      "        \"weapons\": {\n"
      "          \"rifle\": {\n"
      "            \"name\": \"Rifle\",\n"
      "            \"damage_type\": \"small_caliber\",\n"
      "            \"damage\": 180,\n"
      "            \"min_range\": 1,\n"
      "            \"max_range\": 1,\n"
      "            \"ammo\": 6\n"
      "          }\n"
      "        },\n"
      "        \"container\": {\n"
      "          \"max_units\": 0,\n"
      "          \"max_weight\": 0,\n"
      "          \"supported_levels\": []\n"
      "        },\n"
      "        \"value\": 800,\n"
      "        \"view_range\": 4,\n"
      "        \"jamming_range\": 1\n"
      "      }\n"
      "    },\n"
      "    \"map\": {\n"
      "      \"grid\": \"file:map.png\",\n"
      "      \"decorations\": {\n"
      "        \"deco001\": {\n"
      "          \"name\": \"flowers\",\n"
      "          \"index\": 1\n"
      "        }\n"
      "      }\n"
      "    }\n"
      "  },\n"
      "  \"teams\": {},\n"
      "  \"players\": {"
      "    \"player001\": {\n"
      "      \"name\": \"my_player\",\n"
      "      \"color\": {\n"
      "        \"red\": 255,\n"
      "        \"green\": 0,\n"
      "        \"blue\": 0\n"
      "      },\n"
      "      \"fow\": \"file:fow.1.bin\",\n"
      "      \"is_eliminated\": false,\n"
      "      \"lose_conditions\": [],\n"
      "      \"wealth\": 0\n"
      "    }\n"
      "  },\n"
      "  \"shops\": {},\n"
      "  \"units\": {},\n"
      "  \"turn\": 1,\n"
      "  \"player_at_turn\": \"player001\"\n"
      "}\n";

  freeisle::json::test::check(standaloneResult, standaloneExpected);
}

TEST_F(TestSerialize, LoadScenario) {
  freeisle::state::serialize::SerializableState state =
      freeisle::state::serialize::load(
          freeisle::fs::path::join(orig_directory, "data", "state.json")
              .c_str(),
          system.logger.make_child_logger("test"));

  ASSERT_NE(state.scenario, nullptr);

  const freeisle::def::Scenario &scenario = *state.scenario;
  const freeisle::state::State &game_state = state.state;

  EXPECT_EQ(scenario.name, "My Scenario");
  EXPECT_EQ(scenario.description, "East End Boys and West End Girls");
  ASSERT_EQ(scenario.map.grid.width(), 10);
  ASSERT_EQ(scenario.map.grid.height(), 10);
  for (uint32_t y = 0; y < 10; ++y) {
    for (uint32_t x = 0; x < 10; ++x) {
      EXPECT_EQ(scenario.map.grid(x, y).base_terrain,
                freeisle::def::BaseTerrainType::Grass);
      EXPECT_FALSE(scenario.map.grid(x, y).overlay_terrain);
      EXPECT_EQ(scenario.map.grid(x, y).decoration, nullptr);
    }
  }
  EXPECT_EQ(scenario.map.decoration_defs.size(), 1);
  ASSERT_EQ(scenario.map.decoration_defs.count("deco001"), 1);
  EXPECT_EQ(scenario.map.decoration_defs.find("deco001")->second.name,
            "flowers");
  EXPECT_EQ(scenario.units.size(), 1);
  ASSERT_EQ(scenario.units.count("unitdef001"), 1);

  const freeisle::def::Collection<freeisle::def::UnitDef>::const_iterator
      unit_def_iter = scenario.units.find("unitdef001");
  ASSERT_NE(unit_def_iter, scenario.units.end());
  const freeisle::def::UnitDef &unit = unit_def_iter->second;

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
      weapon_iter = unit.weapons.find("rifle");
  ASSERT_NE(weapon_iter, unit.weapons.end());
  EXPECT_EQ(weapon_iter->second.name, "Rifle");
  EXPECT_EQ(weapon_iter->second.damage_type,
            freeisle::def::DamageType::SmallCaliber);
  EXPECT_EQ(weapon_iter->second.damage, 180);
  EXPECT_EQ(weapon_iter->second.min_range, 1);
  EXPECT_EQ(weapon_iter->second.max_range, 1);
  EXPECT_EQ(weapon_iter->second.ammo, 6);

  EXPECT_EQ(unit.container.max_units, 0);
  EXPECT_EQ(unit.container.max_weight, 0);
  EXPECT_EQ(unit.container.supported_levels,
            freeisle::core::Bitmask<freeisle::def::Level>{});
  EXPECT_EQ(unit.value, 800);
  EXPECT_EQ(unit.view_range, 4);
  EXPECT_EQ(unit.jamming_range, 1);

  EXPECT_EQ(game_state.scenario, &scenario);
  EXPECT_EQ(game_state.teams.size(), 0);
  ASSERT_EQ(game_state.players.size(), 1);
  ASSERT_EQ(game_state.players.count("player001"), 1);
  EXPECT_EQ(game_state.map.grid.width(), 10);
  EXPECT_EQ(game_state.map.grid.height(), 10);
  EXPECT_EQ(game_state.shops.size(), 0);
  EXPECT_EQ(game_state.units.size(), 0);
  EXPECT_EQ(game_state.turn_num, 1);
  EXPECT_EQ(game_state.player_at_turn, game_state.players.begin());

  const freeisle::def::Collection<freeisle::state::Player>::const_iterator
      iter = game_state.players.find("player001");
  ASSERT_NE(iter, game_state.players.end());
  const freeisle::state::Player &player = iter->second;

  EXPECT_EQ(player.name, "my_player");
  EXPECT_EQ(player.color.r, 255);
  EXPECT_EQ(player.color.g, 0);
  EXPECT_EQ(player.color.b, 0);
  EXPECT_FALSE(player.team);
  EXPECT_EQ(player.fow.width(), 10);
  EXPECT_EQ(player.fow.height(), 10);
  for (uint32_t y = 0; y < 10; ++y) {
    for (uint32_t x = 0; x < 10; ++x) {
      EXPECT_EQ(player.fow(x, y).discovered, false);
      EXPECT_EQ(player.fow(x, y).view, 0);
    }
  }
  EXPECT_EQ(player.wealth, 0);
  EXPECT_FALSE(player.captain);
  EXPECT_EQ(player.lose_conditions,
            freeisle::core::Bitmask<freeisle::def::Goal>{});
  EXPECT_EQ(player.is_eliminated, false);
  EXPECT_EQ(player.units.size(), 0);
}
