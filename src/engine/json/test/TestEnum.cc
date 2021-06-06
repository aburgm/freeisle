#include "json/LoadUtil.hh"
#include "json/Loader.hh"
#include "json/SaveUtil.hh"
#include "json/Saver.hh"

#include "core/Enum.hh"

#include "core/test/util/Util.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

namespace {

enum class TestEnum { Enum1, Enum2, Enum3, Num };

constexpr freeisle::core::EnumEntry<TestEnum> entries[] = {
    {TestEnum::Enum1, "Enum1"},
    {TestEnum::Enum2, "Enum2"},
    {TestEnum::Enum3, "Enum3"}};

struct TestStruct {
  TestEnum test;
};

struct TestLoader {
  TestStruct &test;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    test.test = freeisle::json::loader::load_enum(ctx, value, "test", entries);
  }
};

struct TestSaver {
  const TestStruct &test;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save_enum(ctx, value, "test", test.test, entries);
  }
};

} // namespace

TEST(Enum, LoadNotPresent) {
  TestStruct test;
  TestLoader loader{test};

  const std::string text = "{}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Mandatory field \"test\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
  }
}

TEST(Enum, LoadNotExist) {
  TestStruct test;
  TestLoader loader{test};

  const std::string text = "{\"test\": \"Something\"}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Illegal value: \"Something\". Allowed values are: "
                           "Enum1, Enum2, Enum3");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 10);
  }
}

TEST(Enum, LoadExist) {
  TestStruct test;
  TestLoader loader{test};

  const std::string text = "{\"test\": \"Enum2\"}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, loader);

  EXPECT_EQ(test.test, TestEnum::Enum2);
}

TEST(Enum, Save) {
  TestStruct test{.test = TestEnum::Enum3};
  TestSaver saver{test};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);

  const std::string expected = "{\"test\": \"Enum3\"}";
  freeisle::json::test::check(result, expected);
}
