#include "core/EnumMap.hh"
#include "json/SaveUtil.hh"
#include "json/Saver.hh"

#include "json/test/Util.hh"

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
  const TestStruct &test;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::EnumMapSaver<uint32_t, TestEnum> enum_map_saver(
        test.map, names);
    freeisle::json::saver::save_object(ctx, value, "map", enum_map_saver);
  }
};

} // namespace

TEST(EnumMapSaver, Saver) {
  TestStruct test;
  test.map[TestEnum::Enum2] = 10;
  test.map[TestEnum::Enum3] = 22;
  TestHandler handler{test};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"map\": {\"Enum1\": 0, \"Enum2\": 10, \"Enum3\": 22}}";
  freeisle::json::test::check(result, expected);
}
