#include "def/serialize/ShopDefHandlers.hh"

#include "fs/Path.hh"

#include "core/test/util/Util.hh"
#include "log/test/util/System.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

class TestShopDefHandlers : public ::testing::Test {
public:
  TestShopDefHandlers() : aux{system.logger} {}

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;
};

TEST_F(TestShopDefHandlers, Load) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5)};

  freeisle::def::Collection<freeisle::def::UnitDef> unit_defs = {
      {"unit001", freeisle::def::UnitDef{}},
      {"grunt", freeisle::def::UnitDef{}},
      {"unit003", freeisle::def::UnitDef{}},
  };

  const freeisle::def::Collection<freeisle::def::UnitDef>::iterator grunt =
      unit_defs.find("grunt");
  ASSERT_NE(grunt, unit_defs.end());

  freeisle::def::Collection<freeisle::def::ShopDef> shops;
  freeisle::def::ShopDef &shop = shops["shop001"];
  freeisle::def::serialize::ShopDefLoader loader(map, unit_defs, aux);
  loader.set(shops.find("shop001"));

  freeisle::json::loader::load_root_object("data/shop_fayetteville.json",
                                           loader);

  EXPECT_EQ(shop.name, "Fayetteville");
  EXPECT_EQ(shop.type, freeisle::def::ShopDef::Type::Town);
  EXPECT_EQ(shop.income, 350);
  EXPECT_EQ(shop.container.max_units, 4);
  EXPECT_EQ(shop.container.max_weight, 4000);
  EXPECT_EQ(shop.container.supported_levels, freeisle::def::Level::Land);
  EXPECT_EQ(shop.production_list.size(), 1);
  EXPECT_EQ(shop.production_list.count(grunt), 1);
  EXPECT_EQ(shop.location.x, 4);
  EXPECT_EQ(shop.location.y, 3);
}

TEST_F(TestShopDefHandlers, LoadInvalidLocation) {
  const freeisle::def::MapDef map = {
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(4, 4)};

  freeisle::def::Collection<freeisle::def::UnitDef> unit_defs = {
      {"unit001", freeisle::def::UnitDef{}},
      {"grunt", freeisle::def::UnitDef{}},
      {"unit003", freeisle::def::UnitDef{}},
  };

  const freeisle::def::Collection<freeisle::def::UnitDef>::iterator grunt =
      unit_defs.find("grunt");
  ASSERT_NE(grunt, unit_defs.end());

  freeisle::def::Collection<freeisle::def::ShopDef> shops;
  shops.try_emplace("shop001");

  freeisle::def::serialize::ShopDefLoader loader(map, unit_defs, aux);
  loader.set(shops.find("shop001"));

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/shop_fayetteville.json", loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Location is out of bounds; loc=4,3 but bounds=4x4");
    EXPECT_EQ(e.line(), 12);
    EXPECT_EQ(e.col(), 14);
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "shop_fayetteville.json");
  }
}

TEST_F(TestShopDefHandlers, Save) {
  freeisle::def::Collection<freeisle::def::UnitDef> unit_defs = {
      {"unit001", freeisle::def::UnitDef{}},
      {"grunt", freeisle::def::UnitDef{}},
      {"unit003", freeisle::def::UnitDef{}},
  };

  const freeisle::def::Collection<freeisle::def::UnitDef>::iterator grunt =
      unit_defs.find("grunt");
  ASSERT_NE(grunt, unit_defs.end());

  const freeisle::def::Collection<freeisle::def::ShopDef> shop_defs =
      freeisle::def::make_collection<freeisle::def::ShopDef>(std::make_pair(
          "shop001",
          freeisle::def::ShopDef{
              .name = "Fayetteville",
              .type = freeisle::def::ShopDef::Type::Town,
              .income = 350,
              .container = {.max_units = 4,
                            .max_weight = 4000,
                            .supported_levels = freeisle::def::Level::Land},
              .production_list =
                  freeisle::def::make_ref_set<freeisle::def::UnitDef>(grunt),
              .location = {.x = 4, .y = 3}}));

  freeisle::def::serialize::ShopDefSaver saver(unit_defs, aux);
  saver.set(shop_defs.find("shop001"));

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);
  const std::vector<uint8_t> expected =
      freeisle::fs::read_file("data/shop_fayetteville.json", nullptr);

  freeisle::json::test::check(result, expected);
}
