#include "core/Bitmask.hh"
#include "json/SaveUtil.hh"
#include "json/Saver.hh"

#include "json/test/Util.hh"

#include <gtest/gtest.h>

namespace {

enum class TestEnum { Enum1, Enum2, Enum3 };

const freeisle::core::EnumEntry<TestEnum> entries[] = {
    {TestEnum::Enum1, "Enum1"},
    {TestEnum::Enum2, "Enum2"},
    {TestEnum::Enum3, "Enum3"}};

struct TestStruct {
  freeisle::core::Bitmask<TestEnum> mask;
};

struct TestHandler {
  const TestStruct &test;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save_bitmask(ctx, value, "mask", test.mask, entries);
  }
};

} // namespace

TEST(BitmaskSaver, Empty) {
  TestStruct test;
  TestHandler handler{test};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected = "{\"mask\": []}";
  freeisle::json::test::check(result, expected);
}

TEST(BitmaskSaver, OneBit) {
  TestStruct test{.mask{TestEnum::Enum3}};
  TestHandler handler{test};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected = "{\"mask\": [\"Enum3\"]}";
  freeisle::json::test::check(result, expected);
}

TEST(BitmaskSaver, TwoBits) {
  TestStruct test{.mask{TestEnum::Enum3, TestEnum::Enum1}};
  TestHandler handler{test};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected = "{\"mask\": [\"Enum1\", \"Enum3\"]}";
  freeisle::json::test::check(result, expected);
}
