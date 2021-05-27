#include "json/Loader.hh"
#include "json/LoadUtil.hh"

#include "fs/Path.hh"

#include <gtest/gtest.h>

// TODO(armin): this is available in gtest post 1.10:
#define ASSERT_THROW_KEEP_AS_E(statement, expected_exception)                  \
  std::exception_ptr _exceptionPtr;                                            \
  try {                                                                        \
    (statement);                                                               \
    FAIL() << "Expected: " #statement                                          \
              " throws an exception of type " #expected_exception              \
              ".\n  Actual: it throws nothing.";                               \
  } catch (expected_exception const &) {                                       \
    _exceptionPtr = std::current_exception();                                  \
  } catch (...) {                                                              \
    FAIL() << "Expected: " #statement                                          \
              " throws an exception of type " #expected_exception              \
              ".\n  Actual: it throws a different type.";                      \
  }                                                                            \
  try {                                                                        \
    std::rethrow_exception(_exceptionPtr);                                     \
  } catch (expected_exception const &e)

namespace {

template <typename MapT, typename KeyT>
const typename MapT::mapped_type &get_key(const MapT &map, const KeyT &key) {
  const typename MapT::const_iterator iter = map.find(key);
  if (iter == map.end()) {
    throw std::invalid_argument(
        fmt::format("Map does not contain key \"{}\"", key));
  }

  return iter->second;
}

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
  Defg &defg;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    defg.d = freeisle::json::loader::load<uint32_t>(ctx, value, "d");
    defg.e = freeisle::json::loader::load<bool>(ctx, value, "e");
    defg.f = freeisle::json::loader::load<std::string>(ctx, value, "f");
    defg.g = freeisle::json::loader::load<float>(ctx, value, "g");
    defg.h = value.isMember("h");

    if (!defg.e) {
      throw freeisle::json::loader::Error::create(ctx, "e", value["e"],
                                                  "e cannot be false");
    }
  }
};

struct AbcHandler {
  AbcHandler(Abc &abc) : abc(abc), defg_handler{abc.c} {}

  Abc &abc;
  DefgHandler defg_handler;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    abc.a = freeisle::json::loader::load<std::string>(ctx, value, "a");
    abc.b = freeisle::json::loader::load<uint32_t>(ctx, value, "b");
    load_object(ctx, value, "c", defg_handler);
  }
};

} // namespace

TEST(Loader, Simple) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::string text =
      "{\"d\": 54, \"e\": true, \"f\": \"omg\", \"g\": 3.5}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(defg.d, 54);
  EXPECT_EQ(defg.e, true);
  EXPECT_EQ(defg.f, "omg");
  EXPECT_EQ(defg.g, 3.5f);
  EXPECT_EQ(defg.h, false);

  EXPECT_TRUE(include_map.empty());
}

TEST(Loader, SimpleMissingField) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::string text = "{\"d\": 54, \"e\": true, \"f\": \"omg\"}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
  }

  EXPECT_EQ(defg.d, 54);
  EXPECT_EQ(defg.e, true);
  EXPECT_EQ(defg.f, "omg");
  EXPECT_EQ(defg.g, 0.0f);
  EXPECT_EQ(defg.h, false);
}

TEST(Loader, SimpleWrongType) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::string text =
      "{\"d\": 54, \"e\": true, \"f\": \"omg\", \"g\": \"bla\"}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 39);
  }
}

TEST(Loader, SimpleBadSyntax) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::string text =
      "{\"d\": 54, \"e\": true, \"f\": \"omg\", \"g\": \"bla}";
  std::vector<uint8_t> data(text.begin(), text.end());

  EXPECT_THROW(freeisle::json::loader::load_root_object(data, handler),
               std::runtime_error);
}

TEST(Loader, SimpleCustomError) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::string text =
      "{\"d\": 54, \"e\": false, \"f\": \"omg\", \"g\": 3.5}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 16);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, Composite) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::string text = "{\"a\": \"text\", \"b\": 12, \"c\": {\"d\": 54, "
                           "\"e\": true, \"f\": \"omg\", \"g\": 3.5}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(abc.a, "text");
  EXPECT_EQ(abc.b, 12);
  EXPECT_EQ(abc.c.d, 54);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "omg");
  EXPECT_EQ(abc.c.g, 3.5f);
  EXPECT_EQ(abc.c.h, false);

  EXPECT_TRUE(include_map.empty());
}

TEST(Loader, IncludeFromMemory) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::string text = "{\"include\": \"test.json\"}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 13);
    EXPECT_EQ(e.message(), "No search paths available for include resolution");
  }
}

TEST(Loader, SimpleFromFile) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object("data/defg_working.json",
                                               handler);

  EXPECT_EQ(defg.d, 1337);
  EXPECT_EQ(defg.e, true);
  EXPECT_EQ(defg.f, "string");
  EXPECT_EQ(defg.g, 34.2f);
  EXPECT_EQ(defg.h, true);

  EXPECT_TRUE(include_map.empty());
}

TEST(Loader, SimpleFromFileWithNodeRemoval) {
  Defg defg{};
  DefgHandler handler{defg};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object("data/defg_removal.json",
                                               handler);

  EXPECT_EQ(defg.d, 1337);
  EXPECT_EQ(defg.e, true);
  EXPECT_EQ(defg.f, "overridden");
  EXPECT_EQ(defg.g, 34.2f);
  EXPECT_EQ(defg.h, false);

  EXPECT_EQ(include_map.size(), 1);
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      iter = include_map.find("");
  EXPECT_NE(iter, include_map.end());

  EXPECT_EQ(iter->second.filename, "defg_working.json");
  EXPECT_EQ(iter->second.override_keys.size(), 2);
  EXPECT_EQ(get_key(iter->second.override_keys, "f"), true);
  EXPECT_EQ(get_key(iter->second.override_keys, "h"), false);
}

TEST(Loader, SyntaxErrorFromFile) {
  Defg defg{};
  DefgHandler handler{defg};

  EXPECT_THROW(freeisle::json::loader::load_root_object(
                   "data/defg_syntax_error.json", handler),
               std::runtime_error);
}

TEST(Loader, CustomErrorFromFile) {
  Defg defg{};
  DefgHandler handler{defg};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/defg_custom_error.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "defg_custom_error.json");
    EXPECT_EQ(e.line(), 3);
    EXPECT_EQ(e.col(), 8);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, CompositeFromFile) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object("data/abc_working.json",
                                               handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 42);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, false);

  EXPECT_TRUE(include_map.empty());
}

TEST(Loader, CompositeWithSimpleInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_include.json", handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 1337);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, true);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      iter = include_map.find(".c");

  EXPECT_EQ(include_map.size(), 1);
  ASSERT_NE(iter, include_map.end());

  EXPECT_EQ(iter->second.filename, "defg_working.json");
  EXPECT_EQ(iter->second.override_keys.size(), 0);
}

TEST(Loader, CompositeWithSamelevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_samelevel_include.json", handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 1337);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "another string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, true);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      iter = include_map.find(".c");

  EXPECT_EQ(include_map.size(), 1);
  ASSERT_NE(iter, include_map.end());

  EXPECT_EQ(iter->second.filename, "defg_working.json");
  EXPECT_EQ(iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(iter->second.override_keys, "f"), true);
}

TEST(Loader, CompositeWithSublevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_sublevel_include.json", handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 40);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, false);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      top_iter = include_map.find("");
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      sub_iter = include_map.find(".c");

  EXPECT_EQ(include_map.size(), 2);
  ASSERT_NE(top_iter, include_map.end());
  ASSERT_NE(sub_iter, include_map.end());

  EXPECT_EQ(top_iter->second.filename, "abc_working.json");
  EXPECT_EQ(top_iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "c"), true);

  EXPECT_EQ(sub_iter->second.filename, "");
  EXPECT_EQ(sub_iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(sub_iter->second.override_keys, "d"), true);
}

TEST(Loader, CompositeWithMultilevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_multilevel_include.json", handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 1337);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, true);

  EXPECT_EQ(include_map.size(), 2);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      top_iter = include_map.find("");
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      sub_iter = include_map.find(".c");

  EXPECT_EQ(include_map.size(), 2);
  ASSERT_NE(top_iter, include_map.end());
  ASSERT_NE(sub_iter, include_map.end());

  EXPECT_EQ(top_iter->second.filename, "abc_c_override.frag.json");
  EXPECT_EQ(top_iter->second.override_keys.size(), 3);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "a"), true);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "b"), true);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "c"), true);

  EXPECT_EQ(sub_iter->second.filename, "defg_working.json");
  EXPECT_EQ(sub_iter->second.override_keys.size(), 0);
}

TEST(Loader, CompositeWithDoubleInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_double_include.json", handler);

  EXPECT_EQ(abc.a, "yeah");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 40);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, false);

  EXPECT_EQ(include_map.size(), 1);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      iter = include_map.find("");

  EXPECT_EQ(iter->second.filename, "abc_working_with_sublevel_include.json");
  EXPECT_EQ(iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(iter->second.override_keys, "a"), true);
}

TEST(Loader, CompositeWithFixedErrorFromSublevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_fixed_error_from_sublevel_include.json",
          handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 37);
  EXPECT_EQ(abc.c.d, 42);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, false);

  EXPECT_EQ(include_map.size(), 2);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      top_iter = include_map.find("");
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      sub_iter = include_map.find(".c");

  ASSERT_NE(top_iter, include_map.end());
  ASSERT_NE(sub_iter, include_map.end());

  EXPECT_EQ(top_iter->second.filename, "abc_custom_error.json");
  EXPECT_EQ(top_iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "c"), true);

  EXPECT_EQ(sub_iter->second.filename, "");
  EXPECT_EQ(sub_iter->second.override_keys.size(), 1);
  EXPECT_EQ(get_key(sub_iter->second.override_keys, "e"), true);
}

TEST(Loader, CompositeWithFixedErrorFromMultilevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/abc_working_with_fixed_error_from_multilevel_include.json",
          handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 37);
  EXPECT_EQ(abc.c.d, 1337);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, true);

  EXPECT_EQ(include_map.size(), 2);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      top_iter = include_map.find("");
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      sub_iter = include_map.find(".c");

  ASSERT_NE(top_iter, include_map.end());
  ASSERT_NE(sub_iter, include_map.end());

  EXPECT_EQ(top_iter->second.filename, "abc_custom_error.json");
  EXPECT_EQ(top_iter->second.override_keys.size(), 2);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "a"), true);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "c"), true);

  EXPECT_EQ(sub_iter->second.filename, "defg_working.json");
  EXPECT_EQ(sub_iter->second.override_keys.size(), 0);
}

TEST(Loader, CompositeWithFixedErrorFromMultilevelDoubleInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(
          "data/"
          "abc_working_with_fixed_error_from_multilevel_double_include.json",
          handler);

  EXPECT_EQ(abc.a, "hi");
  EXPECT_EQ(abc.b, 31337);
  EXPECT_EQ(abc.c.d, 1337);
  EXPECT_EQ(abc.c.e, true);
  EXPECT_EQ(abc.c.f, "string");
  EXPECT_EQ(abc.c.g, 34.2f);
  EXPECT_EQ(abc.c.h, true);

  EXPECT_EQ(include_map.size(), 2);

  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      top_iter = include_map.find("");
  const std::map<std::string, freeisle::json::IncludeInfo>::const_iterator
      sub_iter = include_map.find(".c");

  ASSERT_NE(top_iter, include_map.end());
  ASSERT_NE(sub_iter, include_map.end());

  EXPECT_EQ(top_iter->second.filename, "abc_custom_error_in_include.json");
  EXPECT_EQ(top_iter->second.override_keys.size(), 2);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "a"), true);
  EXPECT_EQ(get_key(top_iter->second.override_keys, "c"), true);

  EXPECT_EQ(sub_iter->second.filename, "defg_working.json");
  EXPECT_EQ(sub_iter->second.override_keys.size(), 0);
}

TEST(Loader, CircularIncludeDirect) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_circular_include_error_direct.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_circular_include_error_direct.json");
    EXPECT_EQ(e.line(), 2);
    EXPECT_EQ(e.col(), 14);
    EXPECT_EQ(e.message(), "Cyclic include path");
  }
}

TEST(Loader, CircularIncludeIndirect) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_circular_include_error_indirect.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_circular_include_error_indirect.frag.json");
    EXPECT_EQ(e.line(), 3);
    EXPECT_EQ(e.col(), 14);
    EXPECT_EQ(e.message(), "Cyclic include path");
  }
}

TEST(Loader, NotFoundError) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/abc_not_found_error.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_not_found_error.json");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 16);
  }
}

TEST(Loader, NotFoundErrorWithDoubleInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_not_found_error_with_double_include.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_not_found_error.json");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 16);
  }
}

TEST(Loader, SyntaxErrorInInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/abc_syntax_error_in_include.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_syntax_error_in_include.json");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 16);
  }
}

TEST(Loader, WrongPathError) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/abc_wrong_path_error.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_wrong_path_error.json");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 16);
    EXPECT_EQ(e.message(),
              "\"../data/defg_working.json\" would traverse top directory");
  }
}

TEST(Loader, AbsolutePathError) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/abc_absolute_path_error.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_absolute_path_error.json");
    EXPECT_EQ(e.line(), 5);
    EXPECT_EQ(e.col(), 16);
    EXPECT_EQ(e.message(), "Include path cannot be absolute");
  }
}

TEST(Loader, CustomErrorInInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(
                             "data/abc_custom_error_in_include.json", handler),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "defg_custom_error.json");
    EXPECT_EQ(e.line(), 3);
    EXPECT_EQ(e.col(), 8);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, CustomErrorWithSublevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_custom_error_with_sublevel_include.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()),
              "abc_custom_error_with_sublevel_include.json");
    EXPECT_EQ(e.line(), 4);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, CustomErrorWithMultilevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_custom_error_with_multilevel_include.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "defg_custom_error.json");
    EXPECT_EQ(e.line(), 3);
    EXPECT_EQ(e.col(), 8);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, CustomErrorWithMultilevelDoubleInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_custom_error_with_multilevel_double_include.json", handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "defg_custom_error.json");
    EXPECT_EQ(e.line(), 3);
    EXPECT_EQ(e.col(), 8);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}

TEST(Loader, CustomErrorInheritedFromSublevelInclude) {
  Abc abc{};
  AbcHandler handler{abc};

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(
          "data/abc_custom_error_inherited_from_sublevel_include.json",
          handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(freeisle::fs::path::basename(e.path()), "abc_custom_error.json");
    EXPECT_EQ(e.line(), 6);
    EXPECT_EQ(e.col(), 10);
    EXPECT_EQ(e.message(), "e cannot be false");
  }
}
