#include "core/Bitmask.hh"
#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "core/test/util/Util.hh"

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
  TestStruct &test;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    test.mask =
        freeisle::json::loader::load_bitmask(ctx, value, "mask", entries);
  }
};

} // namespace

TEST(BitmaskLoader, NotExist) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Mandatory field \"mask\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
  }

  EXPECT_FALSE(test.mask.are_any_set());
}

TEST(BitmaskLoader, Null) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": null}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Field \"mask\" is not of array type");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 10);
  }

  EXPECT_FALSE(test.mask.are_any_set());
}

TEST(BitmaskLoader, WrongType) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": {}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Field \"mask\" is not of array type");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 10);
  }

  EXPECT_FALSE(test.mask.are_any_set());
}

TEST(BitmaskLoader, Empty) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": []}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, handler);
  EXPECT_FALSE(test.mask.are_any_set());
}

TEST(BitmaskLoader, OneBit) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": [\"Enum3\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, handler);
  EXPECT_FALSE(test.mask.is_set(TestEnum::Enum1));
  EXPECT_FALSE(test.mask.is_set(TestEnum::Enum2));
  EXPECT_TRUE(test.mask.is_set(TestEnum::Enum3));
}

TEST(BitmaskLoader, TwoBits) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": [\"Enum3\", \"Enum1\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, handler);
  EXPECT_TRUE(test.mask.is_set(TestEnum::Enum1));
  EXPECT_FALSE(test.mask.is_set(TestEnum::Enum2));
  EXPECT_TRUE(test.mask.is_set(TestEnum::Enum3));
}

TEST(BitmaskLoader, NonexistingEnum) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": [\"Enum3\", \"Enum4\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(
        e.message(),
        "Illegal value: \"Enum4\". Allowed values are: Enum1, Enum2, Enum3");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 20);
  }

  EXPECT_FALSE(test.mask.are_any_set());
}

TEST(BitmaskLoader, WrongEnumType) {
  TestStruct test;
  TestHandler handler{test};

  const std::string text = "{\"mask\": [\"Enum3\", []]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Type is not convertible to string");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 20);
  }

  EXPECT_FALSE(test.mask.are_any_set());
}
