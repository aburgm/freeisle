#include "core/Bitmask.hh"

#include <gtest/gtest.h>

enum class TestEnum {
  Test1,
  Test2,
  Test3,
};

TEST(Bitmask, DefaultCtor) {
  freeisle::core::Bitmask<TestEnum> mask;
  EXPECT_FALSE(mask.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask.is_set(TestEnum::Test3));
  EXPECT_FALSE(mask.are_any_set());
  EXPECT_EQ(mask.n_set(), 0);
}

TEST(Bitmask, DefaultInit) {
  freeisle::core::Bitmask<TestEnum> mask{};
  EXPECT_FALSE(mask.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask.is_set(TestEnum::Test3));
  EXPECT_FALSE(mask.are_any_set());
  EXPECT_EQ(mask.n_set(), 0);
}

TEST(Bitmask, SingleBit) {
  freeisle::core::Bitmask<TestEnum> mask1(TestEnum::Test1);
  EXPECT_TRUE(mask1.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask1.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask1.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask1.are_any_set());
  EXPECT_EQ(mask1.n_set(), 1);

  freeisle::core::Bitmask<TestEnum> mask2(TestEnum::Test2);
  EXPECT_FALSE(mask2.is_set(TestEnum::Test1));
  EXPECT_TRUE(mask2.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask2.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask2.are_any_set());
  EXPECT_EQ(mask2.n_set(), 1);

  freeisle::core::Bitmask<TestEnum> mask3(TestEnum::Test3);
  EXPECT_FALSE(mask3.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask3.is_set(TestEnum::Test2));
  EXPECT_TRUE(mask3.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask3.are_any_set());
  EXPECT_EQ(mask3.n_set(), 1);
}

TEST(Bitmask, MultipleBits) {
  freeisle::core::Bitmask<TestEnum> mask1(TestEnum::Test1, TestEnum::Test2);
  EXPECT_TRUE(mask1.is_set(TestEnum::Test1));
  EXPECT_TRUE(mask1.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask1.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask1.are_any_set());
  EXPECT_EQ(mask1.n_set(), 2);

  freeisle::core::Bitmask<TestEnum> mask2(TestEnum::Test1, TestEnum::Test3);
  EXPECT_TRUE(mask2.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask2.is_set(TestEnum::Test2));
  EXPECT_TRUE(mask2.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask2.are_any_set());
  EXPECT_EQ(mask2.n_set(), 2);

  freeisle::core::Bitmask<TestEnum> mask3 = mask1;

  EXPECT_FALSE(mask1.are_all_set(mask2));
  EXPECT_TRUE(mask1.are_all_set(mask3));
  EXPECT_TRUE(mask1.are_all_set(TestEnum::Test1));
  EXPECT_TRUE(mask1.are_all_set(TestEnum::Test2));
  EXPECT_FALSE(mask1.are_all_set(TestEnum::Test3));
  EXPECT_TRUE(mask1.are_any_set(mask2));
  EXPECT_TRUE(mask1.are_any_set(mask3));
  EXPECT_TRUE(mask1.are_any_set(TestEnum::Test1));
  EXPECT_TRUE(mask1.are_any_set(TestEnum::Test2));
  EXPECT_FALSE(mask1.are_any_set(TestEnum::Test3));
}

TEST(Bitmask, ToggleBits) {
  freeisle::core::Bitmask<TestEnum> mask(TestEnum::Test1);

  mask.toggle(TestEnum::Test3);
  EXPECT_TRUE(mask.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask.is_set(TestEnum::Test2));
  EXPECT_TRUE(mask.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask.are_any_set());
  EXPECT_EQ(mask.n_set(), 2);

  mask.toggle(TestEnum::Test1);
  EXPECT_FALSE(mask.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask.is_set(TestEnum::Test2));
  EXPECT_TRUE(mask.is_set(TestEnum::Test3));
  EXPECT_TRUE(mask.are_any_set());
  EXPECT_EQ(mask.n_set(), 1);

  mask.toggle(TestEnum::Test3);
  EXPECT_FALSE(mask.is_set(TestEnum::Test1));
  EXPECT_FALSE(mask.is_set(TestEnum::Test2));
  EXPECT_FALSE(mask.is_set(TestEnum::Test3));
  EXPECT_FALSE(mask.are_any_set());
  EXPECT_EQ(mask.n_set(), 0);
}
