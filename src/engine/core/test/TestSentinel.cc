#include "core/Sentinel.hh"

#include <gtest/gtest.h>

TEST(Sentinel, Default) {
  freeisle::core::Sentinel<int, 0xffff> f;
  EXPECT_FALSE(f);
  EXPECT_TRUE(!f);
}

TEST(Sentinel, Construct) {
  freeisle::core::Sentinel<int, 0xffff> f(7);
  EXPECT_TRUE(f);
  EXPECT_FALSE(!f);
}

TEST(Sentinel, Assign) {
  freeisle::core::Sentinel<int, 0xffff> f;
  f = 7;

  EXPECT_TRUE(f);
  EXPECT_FALSE(!f);

  EXPECT_EQ(*f, 7);
  f = 5;
  EXPECT_EQ(*f, 5);

  // Not really useful because sentinel can basically only be used with
  // primitive types.
  const int *p = f.operator->();
  EXPECT_NE(p, nullptr);
  EXPECT_EQ(*p, 5);

  f = freeisle::core::Sentinel<int, 0xffff>();
  EXPECT_FALSE(f);
  EXPECT_TRUE(!f);
}
