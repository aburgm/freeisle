#include "json/Saver.hh"
#include "json/SaveUtil.hh"

#include "fs/File.hh"
#include "fs/test/util/TempDirFixture.hh"
#include "json/test/Util.hh"

#include <gtest/gtest.h>

namespace {

struct Defg {
  uint32_t d;
  bool e;
  std::string f;
  float g;
  bool h;
};

struct Abc {
  std::string a;
  uint32_t b;
  Defg c;
};

struct DefgHandler {
  const Defg &defg;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save<uint32_t>(ctx, value, "d", defg.d);
    freeisle::json::saver::save<bool>(ctx, value, "e", defg.g);
    freeisle::json::saver::save<std::string>(ctx, value, "f", defg.f);
    freeisle::json::saver::save<float>(ctx, value, "g", defg.g);

    if (defg.h) {
      freeisle::json::saver::save<bool>(ctx, value, "h", defg.h);
    }
  }
};

struct AbcHandler {
  AbcHandler(const Abc &abc) : abc(abc), defg_handler{abc.c} {}

  const Abc &abc;
  DefgHandler defg_handler;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save<std::string>(ctx, value, "a", abc.a);
    freeisle::json::saver::save<uint32_t>(ctx, value, "b", abc.b);
    save_object(ctx, value, "c", defg_handler);
  }
};

class SaverFileTest : public freeisle::fs::test::TempDirFixture {};

} // namespace

TEST(Saver, SimpleWithoutIncludes) {
  const Defg defg{.d = 54, .e = true, .f = "omg", .g = 3.5f};
  DefgHandler handler{defg};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"d\": 54, \"e\": true, \"f\": \"omg\", \"g\": 3.5}";
  freeisle::json::test::check(result, expected);
}

TEST_F(SaverFileTest, SimpleToFile) {
  const Defg defg{.d = 54, .e = true, .f = "omg", .g = 3.5f};
  DefgHandler handler{defg};

  freeisle::json::saver::save_root_object("test.json", handler, nullptr);
  const std::vector<uint8_t> result =
      freeisle::fs::read_file("test.json", nullptr);

  const std::string expected =
      "{\"d\": 54, \"e\": true, \"f\": \"omg\", \"g\": 3.5}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, SimpleWithRootInclude) {
  const Defg defg{.d = 54, .e = true, .f = "overridden", .g = 3.5f, .h = true};
  DefgHandler handler{defg};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"", freeisle::json::IncludeInfo{.filename = "defg_working.json",
                                       .override_keys = {
                                           {"h", false},
                                           {"f", true},
                                       }}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected = "{\"include\": \"defg_working.json\", \"h\": "
                               "null, \"f\": \"overridden\"}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithoutIncludes) {
  const Abc abc{
      .a = "text", .b = 12, .c = {.d = 54, .e = true, .f = "omg", .g = 3.5f}};
  AbcHandler handler{abc};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected = "{\"a\": \"text\", \"b\": 12, \"c\": {\"d\": "
                               "54, \"e\": true, \"f\": \"omg\", \"g\": 3.5}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithSimpleInclude) {
  const Abc abc{
      .a = "hi",
      .b = 31337,
      .c = {.d = 1337, .e = true, .f = "string", .g = 34.2f, .h = true}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {".c", freeisle::json::IncludeInfo{.filename = "defg_working.json",
                                         .override_keys = {}}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected = "{\"a\": \"hi\", \"b\": 31337, \"c\": { "
                               "\"include\": \"defg_working.json\"}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithSamelevelInclude) {
  const Abc abc{
      .a = "hi",
      .b = 31337,
      .c = {
          .d = 1337, .e = true, .f = "another string", .g = 34.2f, .h = true}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {".c", freeisle::json::IncludeInfo{.filename = "defg_working.json",
                                         .override_keys = {
                                             {"f", true},
                                         }}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{\"a\": \"hi\", \"b\": 31337, \"c\": { \"include\": "
      "\"defg_working.json\", \"f\": \"another string\"}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithSublevelInclude) {
  const Abc abc{
      .a = "hi",
      .b = 31337,
      .c = {.d = 40, .e = true, .f = "string", .g = 34.2f, .h = false}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"", freeisle::json::IncludeInfo{.filename = "abc_working.json",
                                       .override_keys =
                                           {
                                               {"c", true},
                                           }}},
      {".c", freeisle::json::IncludeInfo{.filename = "",
                                         .override_keys = {{"d", true}}}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{ \"include\": \"abc_working.json\", \"c\": { \"d\": 40}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithMultilevelInclude) {
  const Abc abc{
      .a = "hi",
      .b = 31337,
      .c = {.d = 1337, .e = true, .f = "string", .g = 34.2, .h = true}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"", freeisle::json::IncludeInfo{.filename = "abc_c_override.frag.json",
                                       .override_keys =
                                           {
                                               {"a", true},
                                               {"b", true},
                                               {"c", true},
                                           }}},
      {".c", freeisle::json::IncludeInfo{.filename = "defg_working.json"}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{ \"include\": \"abc_c_override.frag.json\", \"a\": \"hi\", \"b\": "
      "31337, \"c\": { \"include\": \"defg_working.json\"}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithDoubleInclude) {
  const Abc abc{
      .a = "yeah",
      .b = 31337,
      .c = {.d = 40, .e = true, .f = "string", .g = 34.2, .h = false}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"",
       freeisle::json::IncludeInfo{.filename =
                                       "abc_working_with_sublevel_include.json",
                                   .override_keys =
                                       {
                                           {"a", true},
                                       }}},
  };

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{ \"include\": \"abc_working_with_sublevel_include.json\", \"a\": "
      "\"yeah\"}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithFixedErrorFromSublevelInclude) {
  const Abc abc{
      .a = "hi",
      .b = 37,
      .c = {.d = 42, .e = true, .f = "string", .g = 34.2f, .h = false}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"", freeisle::json::IncludeInfo{.filename = "abc_custom_error.json",
                                       .override_keys =
                                           {
                                               {"c", true},
                                           }}},
      {".c", freeisle::json::IncludeInfo{.filename = "",
                                         .override_keys = {{"e", true}}}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{ \"include\": \"abc_custom_error.json\", \"c\": { \"e\": true}}";
  freeisle::json::test::check(result, expected);
}

TEST(Saver, CompositeWithFixedErrorFromMultilevelInclude) {
  const Abc abc{
      .a = "hi",
      .b = 37,
      .c = {.d = 1337, .e = true, .f = "string", .g = 34.2f, .h = true}};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map = {
      {"", freeisle::json::IncludeInfo{.filename = "abc_custom_error.json",
                                       .override_keys =
                                           {
                                               {"a", true},
                                               {"c", true},
                                           }}},
      {".c", freeisle::json::IncludeInfo{.filename = "defg_working.json"}}};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, &include_map);

  const std::string expected =
      "{ \"include\": \"abc_custom_error.json\", \"a\": \"hi\", \"c\": { "
      "\"include\": \"defg_working.json\"}}";
  freeisle::json::test::check(result, expected);
}
