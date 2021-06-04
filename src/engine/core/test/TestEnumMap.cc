#include "core/Enum.hh"
#include "core/EnumMap.hh"

#include <set>

#include <gtest/gtest.h>

enum class TestEnum {
  Test1,
  Test2,
  Test3,
  Num,
};

enum class TastEnum {
  Tast1,
  Tast2,
  Num,
};

enum class TostEnum {
  Tost1,
  Tost2,
  Num,
};

const freeisle::core::EnumEntry<TestEnum> test_enum_entries[] = {
    {TestEnum::Test1, "Test1"},
    {TestEnum::Test2, "Test2"},
    {TestEnum::Test3, "Test3"},
};

const freeisle::core::EnumEntry<TastEnum> tast_enum_entries[] = {
    {TastEnum::Tast1, "Tast1"},
    {TastEnum::Tast2, "Tast2"},
};

const freeisle::core::EnumEntry<TostEnum> tost_enum_entries[] = {
    {TostEnum::Tost1, "Tost1"},
    {TostEnum::Tost2, "Tost2"},
};

TEST(EnumMap, SingleEnum) {
  freeisle::core::EnumMap<uint32_t, TestEnum> map;
  ASSERT_EQ(map.size(), 3);

  map[TestEnum::Test1] = 6;
  map[TestEnum::Test2] = 3;
  map[TestEnum::Test3] = 8;

  EXPECT_EQ(map[TestEnum::Test1], 6);
  EXPECT_EQ(map[TestEnum::Test2], 3);
  EXPECT_EQ(map[TestEnum::Test3], 8);

  std::multiset<uint32_t> set(map.data(), map.data() + map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);

  const freeisle::core::EnumMap<uint32_t, TestEnum> &const_map = map;
  set = std::multiset<uint32_t>(const_map.data(),
                                const_map.data() + const_map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);

  freeisle::core::EnumMap<const char *, TestEnum> names =
      get_enum_names(test_enum_entries);
  ASSERT_EQ(names.size(), 3);

  EXPECT_STREQ(names[TestEnum::Test1], "Test1");
  EXPECT_STREQ(names[TestEnum::Test2], "Test2");
  EXPECT_STREQ(names[TestEnum::Test3], "Test3");
}

TEST(EnumMap, TwoEnums) {
  freeisle::core::EnumMap<uint32_t, TestEnum, TastEnum> map;
  ASSERT_EQ(map.size(), 5);

  map[TestEnum::Test1] = 6;
  map[TestEnum::Test2] = 3;
  map[TestEnum::Test3] = 8;
  map[TastEnum::Tast1] = 2;
  map[TastEnum::Tast2] = 10;

  EXPECT_EQ(map[TestEnum::Test1], 6);
  EXPECT_EQ(map[TestEnum::Test2], 3);
  EXPECT_EQ(map[TestEnum::Test3], 8);
  EXPECT_EQ(map[TastEnum::Tast1], 2);
  EXPECT_EQ(map[TastEnum::Tast2], 10);

  std::multiset<uint32_t> set(map.data(), map.data() + map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);
  EXPECT_EQ(set.count(2), 1);
  EXPECT_EQ(set.count(10), 1);

  const freeisle::core::EnumMap<uint32_t, TestEnum, TastEnum> &const_map = map;
  set = std::multiset<uint32_t>(const_map.data(),
                                const_map.data() + const_map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);
  EXPECT_EQ(set.count(2), 1);
  EXPECT_EQ(set.count(10), 1);

  freeisle::core::EnumMap<const char *, TestEnum, TastEnum> names =
      get_enum_names(test_enum_entries, tast_enum_entries);
  ASSERT_EQ(names.size(), 5);

  EXPECT_STREQ(names[TestEnum::Test1], "Test1");
  EXPECT_STREQ(names[TestEnum::Test2], "Test2");
  EXPECT_STREQ(names[TestEnum::Test3], "Test3");
  EXPECT_STREQ(names[TastEnum::Tast1], "Tast1");
  EXPECT_STREQ(names[TastEnum::Tast2], "Tast2");
}

TEST(EnumMap, ThreeEnums) {
  freeisle::core::EnumMap<uint32_t, TestEnum, TastEnum, TostEnum> map;
  ASSERT_EQ(map.size(), 7);

  map[TestEnum::Test1] = 6;
  map[TestEnum::Test2] = 3;
  map[TestEnum::Test3] = 8;
  map[TastEnum::Tast1] = 2;
  map[TastEnum::Tast2] = 10;
  map[TostEnum::Tost1] = 0;
  map[TostEnum::Tost2] = 10;

  EXPECT_EQ(map[TestEnum::Test1], 6);
  EXPECT_EQ(map[TestEnum::Test2], 3);
  EXPECT_EQ(map[TestEnum::Test3], 8);
  EXPECT_EQ(map[TastEnum::Tast1], 2);
  EXPECT_EQ(map[TastEnum::Tast2], 10);
  EXPECT_EQ(map[TostEnum::Tost1], 0);
  EXPECT_EQ(map[TostEnum::Tost2], 10);

  std::multiset<uint32_t> set(map.data(), map.data() + map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);
  EXPECT_EQ(set.count(2), 1);
  EXPECT_EQ(set.count(10), 2);
  EXPECT_EQ(set.count(0), 1);

  const freeisle::core::EnumMap<uint32_t, TestEnum, TastEnum, TostEnum>
      &const_map = map;
  set = std::multiset<uint32_t>(const_map.data(),
                                const_map.data() + const_map.size());
  EXPECT_EQ(set.count(6), 1);
  EXPECT_EQ(set.count(3), 1);
  EXPECT_EQ(set.count(8), 1);
  EXPECT_EQ(set.count(2), 1);
  EXPECT_EQ(set.count(10), 2);
  EXPECT_EQ(set.count(0), 1);
}
