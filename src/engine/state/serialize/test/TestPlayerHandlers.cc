#include "state/serialize/PlayerHandlers.hh"

#include "state/Player.hh"
#include "state/Team.hh"
#include "state/Unit.hh"

#include "fs/Path.hh"

#include "core/test/util/Util.hh"
#include "fs/test/util/TempDirFixture.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestPlayerHandlers : public ::freeisle::fs::test::TempDirFixture {
public:
  TestPlayerHandlers() : aux{system.logger} {}

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;
};

TEST_F(TestPlayerHandlers, LoadTeam) {
  freeisle::state::Team team;
  freeisle::state::serialize::TeamLoader loader;
  loader.set(team);

  freeisle::json::loader::load_root_object(
      freeisle::fs::path::join(orig_directory, "data", "team_north.json")
          .c_str(),
      loader);

  EXPECT_EQ(team.name, "North");
}

TEST_F(TestPlayerHandlers, SaveTeam) {
  const freeisle::state::Team team = {.name = "North"};
  freeisle::state::serialize::TeamSaver saver;
  saver.set(team);

  freeisle::json::saver::save_root_object("result.json", saver, nullptr);

  const std::vector<uint8_t> result =
      freeisle::fs::read_file("result.json", nullptr);
  const std::vector<uint8_t> expected = freeisle::fs::read_file(
      freeisle::fs::path::join(orig_directory, "data", "team_north.json")
          .c_str(),
      nullptr);

  freeisle::json::test::check(result, expected);
}

TEST_F(TestPlayerHandlers, LoadPlayer) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{.owner = player}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::state::serialize::PlayerLoader loader(map, teams, units, aux);
  loader.set(*player);

  freeisle::json::loader::load_root_object(
      freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
          .c_str(),
      loader);

  EXPECT_EQ(player->name, "Rose");
  EXPECT_EQ(player->color.r, 100);
  EXPECT_EQ(player->color.g, 50);
  EXPECT_EQ(player->color.b, 10);
  ASSERT_TRUE(player->team);
  EXPECT_EQ(&*player->team, &teams["south"]);
  ASSERT_EQ(player->fow.width(), 5);
  ASSERT_EQ(player->fow.height(), 5);

  for (uint32_t y = 0; y < 5; ++y) {
    for (uint32_t x = 0; x < 5; ++x) {
      EXPECT_EQ(player->fow(x, y).discovered, x == 4 && y == 0);
    }
  }

  EXPECT_EQ(player->wealth, 1000);
  ASSERT_TRUE(player->captain);
  EXPECT_EQ(&*player->captain, &units["unit003"]);
  EXPECT_EQ(player->lose_conditions,
            freeisle::core::Bitmask<freeisle::def::Goal>(
                freeisle::def::Goal::ConquerHq,
                freeisle::def::Goal::EliminateCaptain));
  EXPECT_EQ(player->is_eliminated, false);
  ASSERT_EQ(player->units.size(), 2);
  EXPECT_EQ(player->units.count(units.find("unit001")), 1);
  EXPECT_EQ(player->units.count(units.find("unit002")), 0);
  EXPECT_EQ(player->units.count(units.find("unit003")), 1);
}

TEST_F(TestPlayerHandlers, LoadPlayerTooLargeGridSize) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(4, 4)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{.owner = player}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::state::serialize::PlayerLoader loader(map, teams, units, aux);
  loader.set(*player);

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
              .c_str(),
          loader),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "FoW field does not have expected length; got 4 but expected 2");
    EXPECT_EQ(e.line(), 9);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "player_rose.json");
  }
}

TEST_F(TestPlayerHandlers, LoadPlayerTooSmallGridSize) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(6, 6)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{.owner = player}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::state::serialize::PlayerLoader loader(map, teams, units, aux);
  loader.set(*player);

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
              .c_str(),
          loader),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "FoW field does not have expected length; got 4 but expected 5");
    EXPECT_EQ(e.line(), 9);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "player_rose.json");
  }
}

TEST_F(TestPlayerHandlers, LoadPlayerCaptainUnowned) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::state::serialize::PlayerLoader loader(map, teams, units, aux);
  loader.set(*player);

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
              .c_str(),
          loader),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit declared has captain has different owner");
    EXPECT_EQ(e.line(), 11);
    EXPECT_EQ(e.col(), 14);
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "player_rose.json");
  }
}

TEST_F(TestPlayerHandlers, LoadPlayerCaptainOtherPlayer) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};
  players["player2"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{
                                        .owner = players.find("player2")}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::state::serialize::PlayerLoader loader(map, teams, units, aux);
  loader.set(*player);

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
              .c_str(),
          loader),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Unit declared has captain has different owner");
    EXPECT_EQ(e.line(), 11);
    EXPECT_EQ(e.col(), 14);
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "player_rose.json");
  }
}

TEST_F(TestPlayerHandlers, SavePlayer) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)};

  freeisle::def::Collection<freeisle::state::Player> players;
  players["player"] = freeisle::state::Player{};

  freeisle::def::Ref<freeisle::state::Player> player = players.begin();

  freeisle::def::Collection<freeisle::state::Unit> units =
      freeisle::def::make_collection<freeisle::state::Unit>(
          std::make_pair("unit001", freeisle::state::Unit{.owner = player}),
          std::make_pair("unit002", freeisle::state::Unit{}),
          std::make_pair("unit003", freeisle::state::Unit{.owner = player}));

  freeisle::def::Collection<freeisle::state::Team> teams = {
      {"north", freeisle::state::Team{.name = "North"}},
      {"south", freeisle::state::Team{.name = "South"}},
  };

  freeisle::core::Grid<freeisle::state::Player::Fow> grid(5, 5);
  grid(4, 0).discovered = true;

  *player = freeisle::state::Player{
      .name = "Rose",
      .color =
          {
              .r = 100,
              .g = 50,
              .b = 10,
          },
      .team = teams.find("south"),
      .fow = std::move(grid),
      .wealth = 1000,
      .captain = units.find("unit003"),
      .lose_conditions = {freeisle::def::Goal::ConquerHq,
                          freeisle::def::Goal::EliminateCaptain},
      .is_eliminated = false,
      .units = freeisle::def::make_ref_set<freeisle::state::Unit>(
          units.find("unit001"), units.find("unit003")),
  };

  freeisle::state::serialize::PlayerSaver saver(map, teams, units, aux);
  saver.set(*player);

  freeisle::json::saver::save_root_object("result.json", saver, nullptr);

  const std::vector<uint8_t> result =
      freeisle::fs::read_file("result.json", nullptr);
  const std::vector<uint8_t> expected = freeisle::fs::read_file(
      freeisle::fs::path::join(orig_directory, "data", "player_rose.json")
          .c_str(),
      nullptr);

  freeisle::json::test::check(result, expected);
}
