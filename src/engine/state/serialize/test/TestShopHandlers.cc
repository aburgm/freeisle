#include "state/serialize/ShopHandlers.hh"

#include "state/Shop.hh"

#include "fs/Path.hh"

#include "core/test/util/Util.hh"
#include "fs/test/util/TempDirFixture.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestShopHandlers : public ::freeisle::fs::test::TempDirFixture {
public:
  TestShopHandlers()
      : aux{system.logger},
        map_def{.grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)},
        map{
            .def = &map_def,
            .grid = freeisle::core::Grid<freeisle::state::Map::Hex>(5, 5),
        } {

    shop_defs.try_emplace("shop001",
                          freeisle::def::ShopDef{.location = {.x = 3, .y = 1}});
    shop_defs.try_emplace("shop002",
                          freeisle::def::ShopDef{.location = {.x = 4, .y = 3}});

    players.try_emplace("player001");
    players.try_emplace("player002");
    players.try_emplace("player003");
  }

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;

  const freeisle::def::MapDef map_def;
  freeisle::state::Map map;
  freeisle::def::Collection<freeisle::def::ShopDef> shop_defs;
  freeisle::def::Collection<freeisle::state::Player> players;
};

TEST_F(TestShopHandlers, LoadShop) {
  freeisle::def::Collection<freeisle::state::Shop> shops;
  const freeisle::state::Shop &shop = shops["shop001"];

  freeisle::state::serialize::ShopLoader loader(shop_defs, map, players);
  loader.set(shops.find("shop001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"shop001\",\n"
                                   "  \"owner\": \"player001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_TRUE(shop.owner);
  ASSERT_TRUE(shop.def);
  EXPECT_EQ(&*shop.def, &shop_defs.find("shop001")->second);
  EXPECT_EQ(&*shop.owner, &players.find("player001")->second);

  // Container gets populated by unit loading
  EXPECT_EQ(shop.container.def, &shop_defs.find("shop001")->second.container);
  EXPECT_EQ(shop.container.units.size(), 0);
}

TEST_F(TestShopHandlers, LoadShopUnowned) {
  freeisle::def::Collection<freeisle::state::Shop> shops;
  const freeisle::state::Shop &shop = shops["shop001"];

  freeisle::state::serialize::ShopLoader loader(shop_defs, map, players);
  loader.set(shops.find("shop001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"shop001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_FALSE(shop.owner);
  ASSERT_TRUE(shop.def);
  EXPECT_EQ(&*shop.def, &shop_defs.find("shop001")->second);

  // Container gets populated by unit loading
  EXPECT_EQ(shop.container.def, &shop_defs.find("shop001")->second.container);
  EXPECT_EQ(shop.container.units.size(), 0);
}

TEST_F(TestShopHandlers, LoadShopWithoutDef) {
  freeisle::def::Collection<freeisle::state::Shop> shops;
  shops.try_emplace("shop001");

  freeisle::state::serialize::ShopLoader loader(shop_defs, map, players);
  loader.set(shops.find("shop001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"owner\": \"player001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Mandatory field \"def\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestShopHandlers, LoadShopWithNonexistingDef) {
  freeisle::def::Collection<freeisle::state::Shop> shops;
  shops.try_emplace("shop001");

  freeisle::state::serialize::ShopLoader loader(shop_defs, map, players);
  loader.set(shops.find("shop001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"shop003\",\n"
                                   "  \"owner\": \"player001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Object with ID \"shop003\" does not exist");
    EXPECT_EQ(e.line(), 2);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestShopHandlers, LoadShopAtOccupiedSpot) {
  freeisle::def::Collection<freeisle::state::Unit> units;
  units.try_emplace("unit001",
                    freeisle::state::Unit{.location = {.x = 3, .y = 1}});
  map.grid(3, 1).surface_unit = units.find("unit001");

  freeisle::def::Collection<freeisle::state::Shop> shops;
  shops.try_emplace("shop001");

  freeisle::state::serialize::ShopLoader loader(shop_defs, map, players);
  loader.set(shops.find("shop001"));

  const std::string to_be_loaded = "{\n"
                                   "  \"def\": \"shop001\",\n"
                                   "  \"owner\": \"player001\"\n"
                                   "}\n";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location x=3, y=1 is already occupied");
    EXPECT_EQ(e.line(), 2);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestShopHandlers, Save) {
  freeisle::def::Collection<freeisle::state::Shop> shops;
  shops.try_emplace("shop001",
                    freeisle::state::Shop{.def = shop_defs.find("shop001"),
                                          .owner = players.find("player001")});

  freeisle::state::serialize::ShopSaver saver(shop_defs, players);
  saver.set(shops.find("shop001"));

  freeisle::json::saver::save_root_object("result.json", saver, nullptr);

  const std::vector<uint8_t> result =
      freeisle::fs::read_file("result.json", nullptr);
  const std::string expected = "{\n"
                               "  \"def\": \"shop001\",\n"
                               "  \"owner\": \"player001\"\n"
                               "}\n";

  freeisle::json::test::check(result, expected);
}
