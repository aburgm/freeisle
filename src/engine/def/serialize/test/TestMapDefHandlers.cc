#include "def/serialize/MapDefHandlers.hh"

#include "base64/Base64.hh"
#include "def/serialize/AuxData.hh"
#include "fs/Path.hh"
#include "log/Sink.hh"
#include "log/System.hh"
#include "time/Clock.hh"
#include "json/Saver.hh"
#include "json/test/Util.hh"

#include "core/test/util/Util.hh"
#include "fs/test/util/TempDirFixture.hh"
#include "log/test/util/System.hh"

#include <gtest/gtest.h>

class TestMapDefHandlers : public ::freeisle::fs::test::TempDirFixture {
public:
  TestMapDefHandlers() : aux{system.logger} {}

  freeisle::log::test::System system;
  freeisle::def::serialize::AuxData aux;
};

TEST_F(TestMapDefHandlers, LoadEmpty5x5) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_EQ(map.decoration_defs.size(), 1);
  ASSERT_EQ(map.decoration_defs.begin()->first, "obj001");
  ASSERT_EQ(map.decoration_defs.begin()->second.name, "flowers");
  ASSERT_EQ(map.grid.width(), 5);
  ASSERT_EQ(map.grid.height(), 5);

  for (uint32_t y = 0; y < 5; ++y) {
    for (uint32_t x = 0; x < 5; ++x) {
      SCOPED_TRACE(fmt::format("x={}, y={}", x, y));
      EXPECT_EQ(map.grid(x, y).base_terrain,
                freeisle::def::BaseTerrainType::Grass);
      EXPECT_FALSE(map.grid(x, y).overlay_terrain);
      EXPECT_EQ(map.grid(x, y).decoration, nullptr);
    }
  }
}

TEST_F(TestMapDefHandlers, LoadEmpty5x5FromFile) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": \"file:data/empty_5x5.png\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);

  std::string source = freeisle::fs::path::join(orig_directory, "bla.json");
  std::pair<freeisle::json::loader::Context, Json::Value> pair =
      freeisle::json::loader::make_root_source_context(data, source.c_str());
  loader.load(pair.first, pair.second);

  ASSERT_EQ(map.decoration_defs.size(), 1);
  ASSERT_EQ(map.decoration_defs.begin()->first, "obj001");
  ASSERT_EQ(map.decoration_defs.begin()->second.name, "flowers");
  ASSERT_EQ(map.grid.width(), 5);
  ASSERT_EQ(map.grid.height(), 5);

  for (uint32_t y = 0; y < 5; ++y) {
    for (uint32_t x = 0; x < 5; ++x) {
      SCOPED_TRACE(fmt::format("x={}, y={}", x, y));
      EXPECT_EQ(map.grid(x, y).base_terrain,
                freeisle::def::BaseTerrainType::Grass);
      EXPECT_FALSE(map.grid(x, y).overlay_terrain);
      EXPECT_EQ(map.grid(x, y).decoration, nullptr);
    }
  }
}

TEST_F(TestMapDefHandlers, LoadVarying4x4) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 2}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAIAAAAmkwkpAAAAJUlEQVQImS3GsQ0AIAwDMC"
      "cD//+LRBnAk7MYsKt/igg0ctTkLVzAPAcL8VlLnAAAAABJRU5ErkJggg==\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_EQ(map.decoration_defs.size(), 2);
  freeisle::def::Collection<freeisle::def::DecorationDef>::iterator obj001 =
      map.decoration_defs.find("obj001");
  ASSERT_NE(obj001, map.decoration_defs.end());
  freeisle::def::Collection<freeisle::def::DecorationDef>::iterator obj002 =
      map.decoration_defs.find("obj002");
  ASSERT_NE(obj002, map.decoration_defs.end());
  ASSERT_EQ(obj001->second.name, "flowers");
  ASSERT_EQ(obj002->second.name, "pebbles");
  ASSERT_EQ(map.grid.width(), 4);
  ASSERT_EQ(map.grid.height(), 4);

  EXPECT_EQ(map.grid(0, 0).base_terrain,
            freeisle::def::BaseTerrainType::Mountain);
  EXPECT_EQ(map.grid(1, 0).base_terrain, freeisle::def::BaseTerrainType::Hill);
  EXPECT_EQ(map.grid(2, 0).base_terrain, freeisle::def::BaseTerrainType::Hill);
  EXPECT_EQ(map.grid(3, 0).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(0, 1).base_terrain, freeisle::def::BaseTerrainType::Hill);
  EXPECT_EQ(map.grid(1, 1).base_terrain, freeisle::def::BaseTerrainType::Hill);
  EXPECT_EQ(map.grid(2, 1).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(3, 1).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(0, 2).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(1, 2).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(2, 2).base_terrain, freeisle::def::BaseTerrainType::Snow);
  EXPECT_EQ(map.grid(3, 2).base_terrain,
            freeisle::def::BaseTerrainType::ShallowWater);
  EXPECT_EQ(map.grid(0, 3).base_terrain, freeisle::def::BaseTerrainType::Grass);
  EXPECT_EQ(map.grid(1, 3).base_terrain, freeisle::def::BaseTerrainType::Snow);
  EXPECT_EQ(map.grid(2, 3).base_terrain,
            freeisle::def::BaseTerrainType::ShallowWater);
  EXPECT_EQ(map.grid(3, 3).base_terrain,
            freeisle::def::BaseTerrainType::DeepWater);

  EXPECT_EQ(*map.grid(3, 0).overlay_terrain,
            freeisle::def::OverlayTerrainType::Road);
  EXPECT_EQ(*map.grid(3, 1).overlay_terrain,
            freeisle::def::OverlayTerrainType::Road);
  EXPECT_EQ(*map.grid(2, 2).overlay_terrain,
            freeisle::def::OverlayTerrainType::Road);
  EXPECT_EQ(*map.grid(1, 3).overlay_terrain,
            freeisle::def::OverlayTerrainType::Road);
  EXPECT_EQ(*map.grid(0, 2).overlay_terrain,
            freeisle::def::OverlayTerrainType::Forest);
  EXPECT_EQ(*map.grid(1, 2).overlay_terrain,
            freeisle::def::OverlayTerrainType::Forest);

  EXPECT_EQ(map.grid(0, 3).decoration, &obj001->second);
  EXPECT_EQ(map.grid(2, 1).decoration, &obj002->second);
}

TEST_F(TestMapDefHandlers, LoadCorruptPNG) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": "
      "\"base64:"
      "TAO2Rw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Not a PNG file");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 70);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestMapDefHandlers, LoadUndefinedIndex) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 3}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAIAAAAmkwkpAAAAJUlEQVQImS3GsQ0AIAwDMC"
      "cD//+LRBnAk7MYsKt/igg0ctTkLVzAPAcL8VlLnAAAAABJRU5ErkJggg==\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Map at 2,1: invalid decoration index 2 in B channel");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 113);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestMapDefHandlers, LoadDuplicateIndex) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 1}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Duplicate index");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 100);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestMapDefHandlers, LoadIndex0) {
  // This is because index 0 in the grid means "no decoration".
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 0}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Index 0 is not allowed");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 100);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestMapDefHandlers, LoadTooHighIndex) {
  // This is because index >255 cannot be represented in the PNG encoding.
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 257}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 1}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);
  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Index 256 or greater not allowed");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 57);
    EXPECT_EQ(e.path(), "");
  }
}

TEST_F(TestMapDefHandlers, LoadAbsolutePath) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": \"file:/data/empty_5x5.png\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);

  std::string source = freeisle::fs::path::join(orig_directory, "bla.json");
  std::pair<freeisle::json::loader::Context, Json::Value> pair =
      freeisle::json::loader::make_root_source_context(data, source.c_str());
  ASSERT_THROW_KEEP_AS_E(loader.load(pair.first, pair.second),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Path must not be absolute");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 70);
    EXPECT_EQ(e.path(), source);
  }
}

TEST_F(TestMapDefHandlers, LoadInvalidBaseTerrain) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": \"file:data/invalid_base_terrain_5x5.png\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);

  std::string source = freeisle::fs::path::join(orig_directory, "bla.json");
  std::pair<freeisle::json::loader::Context, Json::Value> pair =
      freeisle::json::loader::make_root_source_context(data, source.c_str());
  ASSERT_THROW_KEEP_AS_E(loader.load(pair.first, pair.second),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Map at 2,2: invalid base terrain type 90 in R channel");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 70);
    EXPECT_EQ(e.path(), source);
  }
}

TEST_F(TestMapDefHandlers, LoadInvalidOverlayTerrain) {
  const std::string to_be_loaded =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": \"file:data/invalid_overlay_terrain_5x5.png\"}";
  const std::vector<uint8_t> data(to_be_loaded.begin(), to_be_loaded.end());

  freeisle::def::MapDef map;
  freeisle::def::serialize::MapDefLoader loader(map, aux);

  std::string source = freeisle::fs::path::join(orig_directory, "bla.json");
  std::pair<freeisle::json::loader::Context, Json::Value> pair =
      freeisle::json::loader::make_root_source_context(data, source.c_str());
  ASSERT_THROW_KEEP_AS_E(loader.load(pair.first, pair.second),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(),
              "Map at 3,2: invalid overlay terrain type 44 in G channel");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 70);
    EXPECT_EQ(e.path(), source);
  }
}

TEST_F(TestMapDefHandlers, SaveEmpty5x5) {
  const freeisle::def::MapDef map = {
      .decoration_defs = {{"obj001", {.name = "flowers"}}},
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5),
  };

  freeisle::def::serialize::MapDefSaver saver(map, aux, "");

  const std::map<std::string, freeisle::json::IncludeInfo> include_map;
  freeisle::json::saver::Context ctx{.include_map = include_map};
  Json::Value value;
  saver.save(ctx, value);

  const std::string expected_value =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImWNgoC4AAABQAA"
      "GmLdqcAAAAAElFTkSuQmCC\"}";
  freeisle::json::test::check(value, expected_value);

  // TODO(armin): load back and compare to original instead?
}

TEST_F(TestMapDefHandlers, SaveEmpty5x5ToFile) {
  const freeisle::def::MapDef map = {
      .decoration_defs =
          {
              {"obj001", {.name = "flowers"}},
          },
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(5, 5),
  };

  freeisle::def::serialize::MapDefSaver saver(map, aux, "map.png");

  const std::map<std::string, freeisle::json::IncludeInfo> include_map;
  freeisle::json::saver::Context ctx{.path = "save.json",
                                     .include_map = include_map};
  Json::Value value;
  saver.save(ctx, value);

  const std::string expected_value =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}}, "
      "\"grid\": \"file:map.png\"}";
  freeisle::json::test::check(value, expected_value);

  const std::vector<uint8_t> file_content =
      freeisle::fs::read_file("map.png", nullptr);
  std::string base64;
  base64.resize(freeisle::base64::round_to_next_multiple_of<4>(
      file_content.size() * 4 / 3));
  uint64_t len =
      freeisle::base64::encode(file_content.data(), file_content.size(),
                               reinterpret_cast<uint8_t *>(&base64[0]));
  base64.resize(len);
  EXPECT_EQ(base64, "iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAIAAAACDbGyAAAADElEQVQImW"
                    "NgoC4AAABQAAGmLdqcAAAAAElFTkSuQmCC");
}

TEST_F(TestMapDefHandlers, SaveVarying4x4) {
  freeisle::def::MapDef map = {
      .decoration_defs =
          {
              {"obj001", {.name = "flowers"}},
              {"obj002", {.name = "pebbles"}},
          },
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(4, 4),
  };

  map.grid(0, 0).base_terrain = freeisle::def::BaseTerrainType::Mountain;
  map.grid(1, 0).base_terrain = freeisle::def::BaseTerrainType::Hill;
  map.grid(2, 0).base_terrain = freeisle::def::BaseTerrainType::Hill;
  map.grid(3, 0).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(0, 1).base_terrain = freeisle::def::BaseTerrainType::Hill;
  map.grid(1, 1).base_terrain = freeisle::def::BaseTerrainType::Hill;
  map.grid(2, 1).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(3, 1).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(0, 2).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(1, 2).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(2, 2).base_terrain = freeisle::def::BaseTerrainType::Snow;
  map.grid(3, 2).base_terrain = freeisle::def::BaseTerrainType::ShallowWater;
  map.grid(0, 3).base_terrain = freeisle::def::BaseTerrainType::Grass;
  map.grid(1, 3).base_terrain = freeisle::def::BaseTerrainType::Snow;
  map.grid(2, 3).base_terrain = freeisle::def::BaseTerrainType::ShallowWater;
  map.grid(3, 3).base_terrain = freeisle::def::BaseTerrainType::DeepWater;

  map.grid(3, 0).overlay_terrain = freeisle::def::OverlayTerrainType::Road;
  map.grid(3, 1).overlay_terrain = freeisle::def::OverlayTerrainType::Road;
  map.grid(2, 2).overlay_terrain = freeisle::def::OverlayTerrainType::Road;
  map.grid(1, 3).overlay_terrain = freeisle::def::OverlayTerrainType::Road;
  map.grid(0, 2).overlay_terrain = freeisle::def::OverlayTerrainType::Forest;
  map.grid(1, 2).overlay_terrain = freeisle::def::OverlayTerrainType::Forest;

  map.grid(0, 3).decoration = &map.decoration_defs.find("obj001")->second;
  map.grid(2, 1).decoration = &map.decoration_defs.find("obj002")->second;

  freeisle::def::serialize::MapDefSaver saver(map, aux, "");

  const std::map<std::string, freeisle::json::IncludeInfo> include_map;
  freeisle::json::saver::Context ctx{.include_map = include_map};
  Json::Value value;
  saver.save(ctx, value);

  const std::string expected_value =
      "{\"decorations\": {\"obj001\": {\"name\": \"flowers\", \"index\": 1}, "
      "\"obj002\": {\"name\": \"pebbles\", \"index\": 2}}, \"grid\": "
      "\"base64:"
      "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAIAAAAmkwkpAAAAJUlEQVQImS3GsQ0AIAwDMC"
      "cD//+LRBnAk7MYsKt/igg0ctTkLVzAPAcL8VlLnAAAAABJRU5ErkJggg==\"}";
  freeisle::json::test::check(value, expected_value);
}

TEST_F(TestMapDefHandlers, SaveTooManyDecorations) {
  // Cannot have more than 255 different decorations or they cannot
  // be represented by 8-bit PNG file.
  freeisle::def::MapDef map = {
      .decoration_defs = {},
      .grid = freeisle::core::Grid<freeisle::def::MapDef::Hex>(4, 4),
  };

  for (uint32_t i = 0; i < 256; ++i) {
    map.decoration_defs[fmt::format("obj{}", i)] =
        freeisle::def::DecorationDef{.name = fmt::format("deco{}", i)};
  }

  freeisle::def::serialize::MapDefSaver saver(map, aux, "");

  const std::map<std::string, freeisle::json::IncludeInfo> include_map;
  freeisle::json::saver::Context ctx{.include_map = include_map};
  Json::Value value;
  ASSERT_THROW_KEEP_AS_E(saver.save(ctx, value), std::runtime_error) {
    EXPECT_STREQ(e.what(), "Too many decoration defs");
  }
}
