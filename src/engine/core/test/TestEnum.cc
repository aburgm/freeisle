#include "core/Enum.hh"

#include <gtest/gtest.h>

enum class TestEnum {
  Test1,
  Test2,
  Test3,
};

const freeisle::core::EnumEntry<TestEnum> test_enum_entries[] = {
    {TestEnum::Test1, "Test1"},
    {TestEnum::Test2, "Test2"},
    {TestEnum::Test3, "Test3"},
};

TEST(Enum, ToString) {
  EXPECT_STREQ(freeisle::core::to_string(test_enum_entries, TestEnum::Test1),
               "Test1");
  EXPECT_STREQ(freeisle::core::to_string(test_enum_entries, TestEnum::Test2),
               "Test2");
  EXPECT_STREQ(freeisle::core::to_string(test_enum_entries, TestEnum::Test3),
               "Test3");
  EXPECT_EQ(freeisle::core::to_string(test_enum_entries,
                                      static_cast<TestEnum>(0xdeadbeef)),
            nullptr);
}

TEST(Enum, FromString) {
  const TestEnum *test1 =
      freeisle::core::from_string(test_enum_entries, "Test1");
  const TestEnum *test2 =
      freeisle::core::from_string(test_enum_entries, "Test2");
  const TestEnum *test3 =
      freeisle::core::from_string(test_enum_entries, "Test3");
  const TestEnum *test4 =
      freeisle::core::from_string(test_enum_entries, "Test4");

  ASSERT_NE(test1, nullptr);
  ASSERT_EQ(*test1, TestEnum::Test1);

  ASSERT_NE(test2, nullptr);
  ASSERT_EQ(*test2, TestEnum::Test2);

  ASSERT_NE(test3, nullptr);
  ASSERT_EQ(*test3, TestEnum::Test3);

  ASSERT_EQ(test4, nullptr);
}
