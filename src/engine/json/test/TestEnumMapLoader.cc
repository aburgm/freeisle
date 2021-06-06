#include "core/EnumMap.hh"
#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "core/test/util/Util.hh"

#include <gtest/gtest.h>

namespace {

enum class TestEnum { Enum1, Enum2, Enum3, Num };

constexpr freeisle::core::EnumEntry<TestEnum> entries[] = {
    {TestEnum::Enum1, "Enum1"},
    {TestEnum::Enum2, "Enum2"},
    {TestEnum::Enum3, "Enum3"}};

constexpr freeisle::core::EnumMap<const char *, TestEnum> names =
    get_enum_names(entries);

struct TestStruct {
  freeisle::core::EnumMap<uint32_t, TestEnum> map{};
};

struct TestHandler {
  TestStruct &test;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    freeisle::json::loader::EnumMapLoader<uint32_t, TestEnum> enum_map_loader(
        test.map, names);
    freeisle::json::loader::load_object(ctx, value, "map", enum_map_loader);
  }
};

} // namespace

TEST(EnumMapLoader, NotExist) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Mandatory field \"map\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
  }

  EXPECT_EQ(test.map[TestEnum::Enum1], 0);
  EXPECT_EQ(test.map[TestEnum::Enum2], 0);
  EXPECT_EQ(test.map[TestEnum::Enum3], 0);
}

TEST(EnumMapLoader, Empty) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"map\": {}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Mandatory field \"Enum1\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 9);
  }

  EXPECT_EQ(test.map[TestEnum::Enum1], 0);
  EXPECT_EQ(test.map[TestEnum::Enum2], 0);
  EXPECT_EQ(test.map[TestEnum::Enum3], 0);
}

TEST(EnumMapLoader, AllPresent) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text =
      "{\"map\": {\"Enum1\": 10, \"Enum2\": 20, \"Enum3\": 54}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(test.map[TestEnum::Enum1], 10);
  EXPECT_EQ(test.map[TestEnum::Enum2], 20);
  EXPECT_EQ(test.map[TestEnum::Enum3], 54);
}
