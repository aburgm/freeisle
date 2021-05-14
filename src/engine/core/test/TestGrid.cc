#include "core/Grid.hh"

#include <gtest/gtest.h>

TEST(Grid, Default) {
  freeisle::core::Grid<int> grid(2, 2);
  EXPECT_EQ(grid(0, 0), 0);
  EXPECT_EQ(grid(0, 1), 0);
  EXPECT_EQ(grid(1, 0), 0);
  EXPECT_EQ(grid(1, 1), 0);
}

TEST(Grid, WriteOneElement) {
  freeisle::core::Grid<int> grid(2, 2);
  grid(0, 1) = 3;

  EXPECT_EQ(grid(0, 0), 0);
  EXPECT_EQ(grid(0, 1), 3);
  EXPECT_EQ(grid(1, 0), 0);
  EXPECT_EQ(grid(1, 1), 0);
}

TEST(Grid, WriteAllElements) {
  freeisle::core::Grid<int> grid(2, 2);
  grid(0, 0) = 1;
  grid(0, 1) = 3;
  grid(1, 0) = 4;
  grid(1, 1) = 2;

  EXPECT_EQ(grid(0, 0), 1);
  EXPECT_EQ(grid(0, 1), 3);
  EXPECT_EQ(grid(1, 0), 4);
  EXPECT_EQ(grid(1, 1), 2);
}

TEST(Grid, Rectangular) {
  freeisle::core::Grid<int> grid(3, 2);
  grid(0, 0) = 1;
  grid(0, 1) = 3;
  grid(1, 0) = 4;
  grid(1, 1) = 2;
  grid(2, 0) = 7;
  grid(2, 1) = 3;

  EXPECT_EQ(grid(0, 0), 1);
  EXPECT_EQ(grid(0, 1), 3);
  EXPECT_EQ(grid(1, 0), 4);
  EXPECT_EQ(grid(1, 1), 2);
  EXPECT_EQ(grid(2, 0), 7);
  EXPECT_EQ(grid(2, 1), 3);
}
