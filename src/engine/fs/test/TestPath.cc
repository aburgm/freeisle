#include "fs/Path.hh"

#include <gtest/gtest.h>

TEST(Path, IsAbsolute) {
  EXPECT_FALSE(freeisle::fs::path::is_absolute(""));
  EXPECT_FALSE(freeisle::fs::path::is_absolute("path"));
  EXPECT_FALSE(freeisle::fs::path::is_absolute("some/path"));
  EXPECT_TRUE(freeisle::fs::path::is_absolute("/"));
  EXPECT_TRUE(freeisle::fs::path::is_absolute("/hello"));
  EXPECT_TRUE(freeisle::fs::path::is_absolute("/some/path"));
}

TEST(Path, Dirname) {
  EXPECT_EQ(freeisle::fs::path::dirname(""), ".");
  EXPECT_EQ(freeisle::fs::path::dirname("."), ".");
  EXPECT_EQ(freeisle::fs::path::dirname(".."), ".");

  EXPECT_EQ(freeisle::fs::path::dirname("hi"), ".");
  EXPECT_EQ(freeisle::fs::path::dirname("hi/"), ".");
  EXPECT_EQ(freeisle::fs::path::dirname("hi/there"), "hi");
  EXPECT_EQ(freeisle::fs::path::dirname("hi/there/"), "hi");
  EXPECT_EQ(freeisle::fs::path::dirname("hi/there//"), "hi");

  EXPECT_EQ(freeisle::fs::path::dirname("/"), "/");
  EXPECT_EQ(freeisle::fs::path::dirname("/hi"), "/");
  EXPECT_EQ(freeisle::fs::path::dirname("/hi/"), "/");
  EXPECT_EQ(freeisle::fs::path::dirname("/hi/there"), "/hi");
  EXPECT_EQ(freeisle::fs::path::dirname("/hi/there/"), "/hi");
  EXPECT_EQ(freeisle::fs::path::dirname("/hi/there//"), "/hi");
}

TEST(Path, Basename) {
  EXPECT_EQ(freeisle::fs::path::basename(""), "");
  EXPECT_EQ(freeisle::fs::path::basename("."), ".");
  EXPECT_EQ(freeisle::fs::path::basename(".."), "..");

  EXPECT_EQ(freeisle::fs::path::basename("hi"), "hi");
  EXPECT_EQ(freeisle::fs::path::basename("hi/"), "hi");
  EXPECT_EQ(freeisle::fs::path::basename("hi/there"), "there");
  EXPECT_EQ(freeisle::fs::path::basename("hi/there/"), "there");
  EXPECT_EQ(freeisle::fs::path::basename("hi/there//"), "there");

  EXPECT_EQ(freeisle::fs::path::basename("/"), "");
  EXPECT_EQ(freeisle::fs::path::basename("/hi"), "hi");
  EXPECT_EQ(freeisle::fs::path::basename("/hi/"), "hi");
  EXPECT_EQ(freeisle::fs::path::basename("/hi/there"), "there");
  EXPECT_EQ(freeisle::fs::path::basename("/hi/there/"), "there");
  EXPECT_EQ(freeisle::fs::path::basename("/hi/there//"), "there");
}

TEST(Path, Extension) {
  EXPECT_EQ(freeisle::fs::path::extension(""), "");
  EXPECT_EQ(freeisle::fs::path::extension("."), ".");
  EXPECT_EQ(freeisle::fs::path::extension(".."), ".");

  EXPECT_EQ(freeisle::fs::path::extension("hi"), "");
  EXPECT_EQ(freeisle::fs::path::extension("hi/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("hi/there"), "");
  EXPECT_EQ(freeisle::fs::path::extension("hi/there/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("hi/there//"), "");

  EXPECT_EQ(freeisle::fs::path::extension("/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there//"), "");

  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there/"), "");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there//"), "");

  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there.gz"), ".gz");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there.gz/"), ".gz");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there.gz//"), ".gz");

  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there."), ".");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there./"), ".");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there.//"), ".");

  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there.."), ".");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there../"), ".");
  EXPECT_EQ(freeisle::fs::path::extension("/hi.tar/there..//"), ".");

  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar"), ".tar");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar/"), ".tar");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar//"), ".tar");

  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar.gz"), ".gz");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar.gz/"), ".gz");
  EXPECT_EQ(freeisle::fs::path::extension("/hi/there.tar.gz//"), ".gz");
}

TEST(Path, Split) {
  using string_pair = std::pair<std::string, std::string>;

  EXPECT_EQ(freeisle::fs::path::split(""), string_pair(".", ""));
  EXPECT_EQ(freeisle::fs::path::split("."), string_pair(".", "."));
  EXPECT_EQ(freeisle::fs::path::split(".."), string_pair(".", ".."));

  EXPECT_EQ(freeisle::fs::path::split("hi"), string_pair(".", "hi"));
  EXPECT_EQ(freeisle::fs::path::split("hi/"), string_pair(".", "hi"));
  EXPECT_EQ(freeisle::fs::path::split("hi/there"), string_pair("hi", "there"));
  EXPECT_EQ(freeisle::fs::path::split("hi/there/"), string_pair("hi", "there"));
  EXPECT_EQ(freeisle::fs::path::split("hi/there//"),
            string_pair("hi", "there"));

  EXPECT_EQ(freeisle::fs::path::split("/"), string_pair("/", ""));
  EXPECT_EQ(freeisle::fs::path::split("/hi"), string_pair("/", "hi"));
  EXPECT_EQ(freeisle::fs::path::split("/hi/"), string_pair("/", "hi"));
  EXPECT_EQ(freeisle::fs::path::split("/hi/there"),
            string_pair("/hi", "there"));
  EXPECT_EQ(freeisle::fs::path::split("/hi/there/"),
            string_pair("/hi", "there"));
  EXPECT_EQ(freeisle::fs::path::split("/hi/there//"),
            string_pair("/hi", "there"));
}

TEST(Path, Resolve) {
  EXPECT_EQ(freeisle::fs::path::resolve(""), "");
  EXPECT_EQ(freeisle::fs::path::resolve("hi"), "hi");
  EXPECT_EQ(freeisle::fs::path::resolve("hi/"), "hi");
  EXPECT_EQ(freeisle::fs::path::resolve("hi//"), "hi");
  EXPECT_EQ(freeisle::fs::path::resolve("hi//there"), "hi/there");
  EXPECT_EQ(freeisle::fs::path::resolve("hi//there/./yeah"), "hi/there/yeah");
  EXPECT_EQ(freeisle::fs::path::resolve("hi/.."), "");
  EXPECT_EQ(freeisle::fs::path::resolve("hi/there/../yeah"), "hi/yeah");
  EXPECT_EQ(freeisle::fs::path::resolve("hi/there/.."), "hi");
  EXPECT_EQ(freeisle::fs::path::resolve("hi/there/../"), "hi");
  EXPECT_EQ(freeisle::fs::path::resolve("hi//there/.../"), "hi/there/...");
  EXPECT_THROW(freeisle::fs::path::resolve(".."), std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::resolve("hi/../.."), std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::resolve(
                   "hi/../../omg/there/really/is/something/there"),
               std::invalid_argument);

  EXPECT_EQ(freeisle::fs::path::resolve("/"), "/");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi"), "/hi");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi/"), "/hi");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi//"), "/hi");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi//there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi//there/./yeah"), "/hi/there/yeah");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi/.."), "/");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi/there/../yeah"), "/hi/yeah");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi/there/.."), "/hi");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi/there/../"), "/hi");
  EXPECT_EQ(freeisle::fs::path::resolve("/hi//there/.../"), "/hi/there/...");
  EXPECT_THROW(freeisle::fs::path::resolve("/.."), std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::resolve("/hi/../.."), std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::resolve(
                   "/hi/../../omg/there/really/is/something/there"),
               std::invalid_argument);
}

TEST(Path, Join) {
  EXPECT_EQ(freeisle::fs::path::join("", ""), "");
  EXPECT_EQ(freeisle::fs::path::join("", "/"), "/");
  EXPECT_EQ(freeisle::fs::path::join("", "hi"), "hi");
  EXPECT_EQ(freeisle::fs::path::join("", "hi/there"), "hi/there");
  EXPECT_EQ(freeisle::fs::path::join("", "/hi/there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("hi", ""), "hi");
  EXPECT_EQ(freeisle::fs::path::join("hi", "/"), "/");
  EXPECT_EQ(freeisle::fs::path::join("hi/there", ""), "hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi/there", ""), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi/there", "/"), "/");
  EXPECT_EQ(freeisle::fs::path::join("/hi/there", "//"), "/");

  EXPECT_EQ(freeisle::fs::path::join("/", "hi/there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi", "hi/there"), "/hi/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi/", "hi/there"), "/hi/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi/", "hi/there"), "/hi/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi//", "hi/there"), "/hi/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi//", "/hi/there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("/hi//", "//hi/there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("hi//", "//hi/there"), "/hi/there");
  EXPECT_EQ(freeisle::fs::path::join("hi//", "hi/there"), "hi/hi/there");

  EXPECT_EQ(freeisle::fs::path::join("more", "than", "two"), "more/than/two");
  EXPECT_EQ(freeisle::fs::path::join("with", "", "gap"), "with/gap");
  EXPECT_EQ(freeisle::fs::path::join("/with", "", "absolute"),
            "/with/absolute");
  EXPECT_EQ(freeisle::fs::path::join("with", "", "/absolute"), "/absolute");
  EXPECT_EQ(freeisle::fs::path::join("/with", "/mid", "absolute"),
            "/mid/absolute");
}

TEST(Path, IsRelativeTo) {
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/", "/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi", "/"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi", "/"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/ther", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/thereyikes", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there/", "/hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there/or/there", "/hi"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi/ther", "/hi/there"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there", "/hi/there"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("/hi/thereyikes", "/hi/there"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there/", "/hi/there"));
  EXPECT_TRUE(
      freeisle::fs::path::is_relative_to("/hi/there/or/there", "/hi/there"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/ther", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/there", "/hi/there"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("hi/thereyikes", "/hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/there/", "/hi/there"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("hi/there/or/there", "/hi/there"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "/hi/there/"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi", "/hi/there/"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi/ther", "/hi/there/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there", "/hi/there/"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("/hi/thereikes", "/hi/there/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("/hi/there/", "/hi/there/"));
  EXPECT_TRUE(
      freeisle::fs::path::is_relative_to("/hi/there/or/there", "/hi/there/"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/ther", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/thereyikes", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there/", "hi"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there/or/there", "hi"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/ther", "hi/there"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/thereyikes", "hi/there"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there/", "hi/there"));
  EXPECT_TRUE(
      freeisle::fs::path::is_relative_to("hi/there/or/there", "hi/there"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi/ther", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi/there", "hi/there"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("/hi/thereyikes", "hi/there"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("/hi/there/", "hi/there"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("/hi/there/or/there", "hi/there"));

  EXPECT_FALSE(freeisle::fs::path::is_relative_to("", "hi/there/"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi", "hi/there/"));
  EXPECT_FALSE(freeisle::fs::path::is_relative_to("hi/ther", "hi/there/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there", "hi/there/"));
  EXPECT_FALSE(
      freeisle::fs::path::is_relative_to("hi/thereyikes", "hi/there/"));
  EXPECT_TRUE(freeisle::fs::path::is_relative_to("hi/there/", "hi/there/"));
  EXPECT_TRUE(
      freeisle::fs::path::is_relative_to("hi/there/or/there", "hi/there/"));
}

TEST(Path, MakeRelative) {
  EXPECT_THROW(freeisle::fs::path::make_relative("", "/"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/", "/"), ".");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi", "/"), "hi");
  EXPECT_THROW(freeisle::fs::path::make_relative("hi", "/"),
               std::invalid_argument);

  EXPECT_THROW(freeisle::fs::path::make_relative("", "/hi"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi", "/hi"), ".");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/ther", "/hi"), "ther");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there", "/hi"), "there");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/thereyikes", "/hi"),
            "thereyikes");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there/", "/hi"), "there/");
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there/or/there", "/hi"),
            "there/or/there");

  EXPECT_THROW(freeisle::fs::path::make_relative("", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/ther", "/hi/there"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there", "/hi/there"), ".");
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/thereyikes", "/hi/there"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there/", "/hi/there"), ".");
  EXPECT_EQ(
      freeisle::fs::path::make_relative("/hi/there/or/there", "/hi/there"),
      "or/there");

  EXPECT_THROW(freeisle::fs::path::make_relative("", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/ther", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/there", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/thereyikes", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/there/", "/hi/there"),
               std::invalid_argument);
  EXPECT_THROW(
      freeisle::fs::path::make_relative("hi/there/or/there", "/hi/there"),
      std::invalid_argument);

  EXPECT_THROW(freeisle::fs::path::make_relative("", "/hi/there/"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi", "/hi/there/"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/ther", "/hi/there/"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there", "/hi/there/"), ".");
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/thereikes", "/hi/there/"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("/hi/there/", "/hi/there/"), ".");
  EXPECT_EQ(
      freeisle::fs::path::make_relative("/hi/there/or/there", "/hi/there/"),
      "or/there");

  EXPECT_THROW(freeisle::fs::path::make_relative("", "hi"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("hi", "hi"), ".");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/ther", "hi"), "ther");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there", "hi"), "there");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/thereyikes", "hi"),
            "thereyikes");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/", "hi"), "there/");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/or/there", "hi"),
            "there/or/there");

  EXPECT_THROW(freeisle::fs::path::make_relative("", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/ther", "hi/there"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there", "hi/there"), ".");
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/thereyikes", "hi/there"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/", "hi/there"), ".");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/or/there", "hi/there"),
            "or/there");

  EXPECT_THROW(freeisle::fs::path::make_relative("", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/ther", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/there", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/thereyikes", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("/hi/there/", "hi/there"),
               std::invalid_argument);
  EXPECT_THROW(
      freeisle::fs::path::make_relative("/hi/there/or/there", "hi/there"),
      std::invalid_argument);

  EXPECT_THROW(freeisle::fs::path::make_relative("", "hi/there/"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi", "hi/there/"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/ther", "hi/there/"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there", "hi/there/"), ".");
  EXPECT_THROW(freeisle::fs::path::make_relative("hi/thereyikes", "hi/there/"),
               std::invalid_argument);
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/", "hi/there/"), ".");
  EXPECT_EQ(freeisle::fs::path::make_relative("hi/there/or/there", "hi/there/"),
            "or/there");
}
