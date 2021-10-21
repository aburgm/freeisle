#include "core/String.hh"

#include <gtest/gtest.h>

TEST(String, HasPrefix) {
  EXPECT_TRUE(freeisle::core::string::has_prefix("", ""));
  EXPECT_TRUE(freeisle::core::string::has_prefix("hi/there", ""));
  EXPECT_FALSE(freeisle::core::string::has_prefix("", "hi/there"));

  EXPECT_TRUE(freeisle::core::string::has_prefix("hi/there", "hi"));
  EXPECT_TRUE(freeisle::core::string::has_prefix("hi/there", "hi/"));
  EXPECT_TRUE(freeisle::core::string::has_prefix("hi/there", "hi/there"));
  EXPECT_FALSE(freeisle::core::string::has_prefix("hi/there", "hi/there/not"));
  EXPECT_FALSE(freeisle::core::string::has_prefix("hi/there", "omg"));
}

TEST(String, HasSuffix) {
  EXPECT_TRUE(freeisle::core::string::has_suffix("", ""));
  EXPECT_TRUE(freeisle::core::string::has_suffix("hi/there", ""));
  EXPECT_FALSE(freeisle::core::string::has_suffix("", "hi/there"));

  EXPECT_TRUE(freeisle::core::string::has_suffix("hi/there", "there"));
  EXPECT_TRUE(freeisle::core::string::has_suffix("hi/there", "/there"));
  EXPECT_TRUE(freeisle::core::string::has_suffix("hi/there", "hi/there"));
  EXPECT_FALSE(freeisle::core::string::has_suffix("hi/there", "not/hi/there"));
  EXPECT_FALSE(freeisle::core::string::has_suffix("hi/there", "omg"));
}

TEST(String, SplitEmptyStr) {
  const std::vector<std::string_view> result =
      freeisle::core::string::split("", "/");
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "");
}

TEST(String, SplitNormal) {
  const std::vector<std::string_view> result =
      freeisle::core::string::split("hi/there//and/then/some", "/");
  ASSERT_EQ(result.size(), 6);
  EXPECT_EQ(result[0], "hi");
  EXPECT_EQ(result[1], "there");
  EXPECT_EQ(result[2], "");
  EXPECT_EQ(result[3], "and");
  EXPECT_EQ(result[4], "then");
  EXPECT_EQ(result[5], "some");
}

TEST(String, SplitMultiSep) {
  const std::vector<std::string_view> result =
      freeisle::core::string::split("hi/there//and/then/some", "//");
  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], "hi/there");
  EXPECT_EQ(result[1], "and/then/some");
}

TEST(String, SplitTripleSep) {
  const std::vector<std::string_view> result =
      freeisle::core::string::split("hi/there//and/then/some", "///");
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "hi/there//and/then/some");
}

TEST(String, JoinTrivial) {
  const std::vector<std::string> components = {};
  EXPECT_EQ(
      freeisle::core::string::join(components.begin(), components.end(), ","),
      "");
}

TEST(String, JoinSingle) {
  const std::vector<std::string> components = {"hi"};
  EXPECT_EQ(
      freeisle::core::string::join(components.begin(), components.end(), ","),
      "hi");
}

TEST(String, JoinMultiple) {
  const std::vector<std::string> components = {"hi", "there", "and", "there"};
  EXPECT_EQ(
      freeisle::core::string::join(components.begin(), components.end(), ","),
      "hi,there,and,there");
}

TEST(String, JoinMultiSep) {
  const std::vector<std::string> components = {"hi", "there", "and", "there"};
  EXPECT_EQ(freeisle::core::string::join(components.begin(), components.end(),
                                         "', '"),
            "hi', 'there', 'and', 'there");
}
