#include "fs/test/util/TempDirFixture.hh"

#include "fs/Directory.hh"

#include <gtest/gtest.h>

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

class DirectoryTest : public freeisle::fs::test::TempDirFixture {};

} // namespace

TEST_F(DirectoryTest, Default) {
  freeisle::fs::Directory dir;
  EXPECT_FALSE(dir);
}

TEST_F(DirectoryTest, OpenExisting) {
  ASSERT_EQ(::mkdir("testDir", 0755), 0);

  freeisle::fs::Directory dir("testDir", nullptr);
  EXPECT_TRUE(dir);
}

TEST_F(DirectoryTest, OpenNonExisting) {
  EXPECT_THROW(freeisle::fs::Directory("testDir", nullptr), std::runtime_error);
}

TEST_F(DirectoryTest, OpenExistingRelative) {
  ASSERT_EQ(::mkdir("testDir", 0755), 0);
  ASSERT_EQ(::mkdir("testDir/another", 0755), 0);

  freeisle::fs::Directory dir("testDir", nullptr);
  EXPECT_TRUE(dir);

  freeisle::fs::Directory subdir("another", &dir);
  EXPECT_TRUE(subdir);
}

TEST_F(DirectoryTest, OpenNonExistingRelative) {
  ASSERT_EQ(::mkdir("testDir", 0755), 0);
  ASSERT_EQ(::mkdir("testDir/another", 0755), 0);

  freeisle::fs::Directory dir("testDir", nullptr);
  EXPECT_TRUE(dir);

  EXPECT_THROW(freeisle::fs::Directory("testDir", &dir), std::runtime_error);
}

TEST_F(DirectoryTest, MoveEmpty) {
  freeisle::fs::Directory dir;
  EXPECT_FALSE(dir);

  freeisle::fs::Directory dir2(std::move(dir));
  EXPECT_FALSE(dir);
  EXPECT_FALSE(dir2);

  dir = std::move(dir2);
  EXPECT_FALSE(dir);
  EXPECT_FALSE(dir2);

  dir = std::move(dir);
  EXPECT_FALSE(dir);
}

TEST_F(DirectoryTest, MoveOpened) {
  ASSERT_EQ(::mkdir("testDir", 0755), 0);
  ASSERT_EQ(::mkdir("testDir2", 0755), 0);
  ASSERT_EQ(::mkdir("testDir2/child", 0755), 0);

  freeisle::fs::Directory dir("testDir", nullptr);
  EXPECT_TRUE(dir);

  dir = std::move(dir);
  EXPECT_TRUE(dir);

  freeisle::fs::Directory dir2(std::move(dir));
  EXPECT_FALSE(dir);
  EXPECT_TRUE(dir2);

  dir = std::move(dir2);
  EXPECT_TRUE(dir);
  EXPECT_FALSE(dir2);

  freeisle::fs::Directory dir3;
  EXPECT_FALSE(dir3);
  dir3 = std::move(dir);
  EXPECT_TRUE(dir3);
  EXPECT_FALSE(dir);

  freeisle::fs::Directory dir4("testDir2", nullptr);
  ASSERT_TRUE(dir4);

  dir3 = std::move(dir4);
  ASSERT_FALSE(dir4);
  ASSERT_TRUE(dir3);

  // dir3 should now point to testDir2 which has a child
  // directory.
  EXPECT_NO_THROW(freeisle::fs::Directory dir4("child", &dir3));
}
