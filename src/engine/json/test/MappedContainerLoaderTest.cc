#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "fs/Path.hh"

#include "core/test/util/Util.hh"

#include <gtest/gtest.h>

namespace {

enum TestEnum { Enum1, Enum2 };

const freeisle::core::EnumEntry<TestEnum> entries[] = {{Enum1, "Enum1"},
                                                       {Enum2, "Enum2"}};

struct Defg {
  uint32_t d;
  TestEnum e;
};

struct Abc {
  std::vector<Defg> defgs;
};

struct DefgHandler {
  Defg *defg;

  void set(Defg &obj) { defg = &obj; }

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    defg->d = freeisle::json::loader::load<uint32_t>(ctx, value, "d");
    defg->e = freeisle::json::loader::load_enum(ctx, value, "e", entries);
  }
};

struct AbcHandler {
  AbcHandler(Abc &abc, std::map<const void *, std::string> *object_id_map)
      : abc(abc), defg_handler(object_id_map) {}

  Abc &abc;
  freeisle::json::loader::MappedContainerHandler<std::vector<Defg>, DefgHandler>
      defg_handler;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    defg_handler.set(abc.defgs);
    load_object(ctx, value, "defgs", defg_handler);
  }
};

} // namespace

TEST(MappedContainerLoader, SuperEmpty) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text = "{}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Mandatory field \"defgs\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
  }

  EXPECT_TRUE(abc.defgs.empty());
}

TEST(MappedContainerLoader, Empty) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text = "{\"defgs\": {}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_TRUE(abc.defgs.empty());
  EXPECT_TRUE(include_map.empty());
}

TEST(MappedContainerLoader, SingleObject) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text =
      "{\"defgs\": {\"obj1\": { \"d\": 30, \"e\": \"Enum1\" }}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(abc.defgs.size(), 1);
  EXPECT_EQ(abc.defgs[0].d, 30);
  EXPECT_EQ(abc.defgs[0].e, Enum1);
  EXPECT_EQ(handler.defg_handler.lookup("obj1"), &abc.defgs[0]);
  EXPECT_EQ(handler.defg_handler.lookup("obj2"), nullptr);

  EXPECT_TRUE(include_map.empty());
}

TEST(MappedContainerLoader, TwoObjects) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text =
      "{\"defgs\": {\"obj1\": { \"d\": 30, \"e\": \"Enum1\" }, \"obj2\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(abc.defgs.size(), 2);
  EXPECT_EQ(abc.defgs[0].d, 30);
  EXPECT_EQ(abc.defgs[0].e, Enum1);
  EXPECT_EQ(abc.defgs[1].d, 35);
  EXPECT_EQ(abc.defgs[1].e, Enum2);
  EXPECT_EQ(handler.defg_handler.lookup("obj1"), &abc.defgs[0]);
  EXPECT_EQ(handler.defg_handler.lookup("obj2"), &abc.defgs[1]);

  EXPECT_TRUE(include_map.empty());
}

TEST(MappedContainerLoader, TwoObjectsWithIdMap) {
  Abc abc;

  std::map<const void *, std::string> object_id_map;
  AbcHandler handler(abc, &object_id_map);

  const std::string text =
      "{\"defgs\": {\"obj1\": { \"d\": 30, \"e\": \"Enum1\" }, \"obj2\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  const std::map<std::string, freeisle::json::IncludeInfo> include_map =
      freeisle::json::loader::load_root_object(data, handler);

  EXPECT_EQ(abc.defgs.size(), 2);
  EXPECT_EQ(abc.defgs[0].d, 30);
  EXPECT_EQ(abc.defgs[0].e, Enum1);
  EXPECT_EQ(abc.defgs[1].d, 35);
  EXPECT_EQ(abc.defgs[1].e, Enum2);
  EXPECT_EQ(handler.defg_handler.lookup("obj1"), &abc.defgs[0]);
  EXPECT_EQ(handler.defg_handler.lookup("obj2"), &abc.defgs[1]);

  EXPECT_TRUE(include_map.empty());

  EXPECT_EQ(object_id_map.size(), 2);
  EXPECT_EQ(object_id_map[&abc.defgs[0]], "obj1");
  EXPECT_EQ(object_id_map[&abc.defgs[1]], "obj2");
}

TEST(MappedContainerLoader, InvalidObjectType) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text = "{\"defgs\": {\"obj1\": 30}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(), "Expected value to be of object type");

    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 20);
  }
}

TEST(MappedContainerLoader, InvalidObject) {
  Abc abc;
  AbcHandler handler(abc, nullptr);

  const std::string text =
      "{\"defgs\": {\"obj1\": { \"d\": 30, \"e\": \"Enum3\" }}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(
      freeisle::json::loader::load_root_object(data, handler),
      freeisle::json::loader::Error) {
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.message(),
              "Illegal value: \"Enum3\". Allowed values are: Enum1, Enum2");

    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 36);
  }
}
