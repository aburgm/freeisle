#include "core/Grid.hh"

#include <gtest/gtest.h>

TEST(Grid, DefaultGrid) {
  const freeisle::core::Grid<int> grid;
  EXPECT_EQ(grid.width(), 0);
  EXPECT_EQ(grid.height(), 0);
}

TEST(Grid, DefaultElements) {
  const freeisle::core::Grid<int> grid(2, 2);
  ASSERT_EQ(grid.width(), 2);
  ASSERT_EQ(grid.height(), 2);
  EXPECT_EQ(grid(0, 0), 0);
  EXPECT_EQ(grid(0, 1), 0);
  EXPECT_EQ(grid(1, 0), 0);
  EXPECT_EQ(grid(1, 1), 0);
}

TEST(Grid, WriteOneElement) {
  freeisle::core::Grid<int> grid(2, 2);
  ASSERT_EQ(grid.width(), 2);
  ASSERT_EQ(grid.height(), 2);

  grid(0, 1) = 3;

  EXPECT_EQ(grid(0, 0), 0);
  EXPECT_EQ(grid(0, 1), 3);
  EXPECT_EQ(grid(1, 0), 0);
  EXPECT_EQ(grid(1, 1), 0);
}

TEST(Grid, WriteAllElements) {
  freeisle::core::Grid<int> grid(2, 2);
  ASSERT_EQ(grid.width(), 2);
  ASSERT_EQ(grid.height(), 2);

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
  ASSERT_EQ(grid.width(), 3);
  ASSERT_EQ(grid.height(), 2);

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

TEST(Grid, MoveConstruction) {
  freeisle::core::Grid<int> grid(3, 2);
  grid(2, 0) = 55;

  freeisle::core::Grid<int> grid2(std::move(grid));

  EXPECT_EQ(grid.width(), 0);
  EXPECT_EQ(grid.height(), 0);

  ASSERT_EQ(grid2.width(), 3);
  ASSERT_EQ(grid2.height(), 2);

  EXPECT_EQ(grid2(0, 0), 0);
  EXPECT_EQ(grid2(0, 1), 0);
  EXPECT_EQ(grid2(1, 0), 0);
  EXPECT_EQ(grid2(1, 1), 0);
  EXPECT_EQ(grid2(2, 0), 55);
  EXPECT_EQ(grid2(2, 1), 0);
}

TEST(Grid, MoveAssignment) {
  freeisle::core::Grid<int> grid(3, 2);
  grid(2, 0) = 55;

  freeisle::core::Grid<int> grid2;
  grid2 = std::move(grid);

  EXPECT_EQ(grid.width(), 0);
  EXPECT_EQ(grid.height(), 0);

  ASSERT_EQ(grid2.width(), 3);
  ASSERT_EQ(grid2.height(), 2);

  EXPECT_EQ(grid2(0, 0), 0);
  EXPECT_EQ(grid2(0, 1), 0);
  EXPECT_EQ(grid2(1, 0), 0);
  EXPECT_EQ(grid2(1, 1), 0);
  EXPECT_EQ(grid2(2, 0), 55);
  EXPECT_EQ(grid2(2, 1), 0);

  freeisle::core::Grid<int> grid3(2, 1);
  grid3(1, 0) = 22;
  grid2 = std::move(grid3);

  EXPECT_EQ(grid3.width(), 0);
  EXPECT_EQ(grid3.height(), 0);

  ASSERT_EQ(grid2.width(), 2);
  ASSERT_EQ(grid2.height(), 1);
  EXPECT_EQ(grid2(0, 0), 0);
  EXPECT_EQ(grid2(1, 0), 22);

  grid2 = std::move(grid2);
  ASSERT_EQ(grid2.width(), 2);
  ASSERT_EQ(grid2.height(), 1);
  EXPECT_EQ(grid2(0, 0), 0);
  EXPECT_EQ(grid2(1, 0), 22);
}
