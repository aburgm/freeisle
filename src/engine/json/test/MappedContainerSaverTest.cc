#include "json/SaveUtil.hh"
#include "json/Saver.hh"

#include "json/test/Util.hh"

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
  const Defg *defg;

  void set(const Defg &obj) { defg = &obj; }

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save<uint32_t>(ctx, value, "d", defg->d);
    freeisle::json::saver::save_enum(ctx, value, "e", defg->e, entries);
  }
};

struct AbcHandler {
  AbcHandler(const Abc &abc, std::map<const void *, std::string> *object_id_map)
      : abc(abc), defg_handler(object_id_map) {}

  const Abc &abc;
  freeisle::json::saver::MappedContainerHandler<std::vector<Defg>, DefgHandler>
      defg_handler;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    defg_handler.set(abc.defgs);
    save_object(ctx, value, "defgs", defg_handler);
  }
};

} // namespace

TEST(MappedContainerSaver, Empty) {
  const Abc abc{.defgs = {}};
  AbcHandler handler(abc, nullptr);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected = "{\"defgs\": {}}";
  freeisle::json::test::check(result, expected);
}

TEST(MappedContainerSaver, SingleObject) {
  const Abc abc = {.defgs = {Defg{.d = 30, .e = Enum1}}};
  AbcHandler handler(abc, nullptr);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"defgs\": {\"obj001\": { \"d\": 30, \"e\": \"Enum1\" }}}";
  freeisle::json::test::check(result, expected);
}

TEST(MappedContainerSaver, TwoObjects) {
  const Abc abc = {
      .defgs = {Defg{.d = 30, .e = Enum1}, Defg{.d = 35, .e = Enum2}}};
  AbcHandler handler(abc, nullptr);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"defgs\": {\"obj001\": { \"d\": 30, \"e\": \"Enum1\" }, \"obj002\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";

  freeisle::json::test::check(result, expected);
}

TEST(MappedContainerSaver, TwoObjectsWithIdMap) {
  const Abc abc = {
      .defgs = {Defg{.d = 30, .e = Enum1}, Defg{.d = 35, .e = Enum2}}};

  std::map<const void *, std::string> object_id_map = {
      {&abc.defgs[0], "fire"},
      {&abc.defgs[1], "ice"},
  };

  AbcHandler handler(abc, &object_id_map);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"defgs\": {\"fire\": { \"d\": 30, \"e\": \"Enum1\" }, \"ice\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";

  freeisle::json::test::check(result, expected);
}

TEST(MappedContainerLoader, TwoObjectsWithPartialIdMap) {
  const Abc abc = {
      .defgs = {Defg{.d = 30, .e = Enum1}, Defg{.d = 35, .e = Enum2}}};

  std::map<const void *, std::string> object_id_map = {
      {&abc.defgs[0], "fire"},
  };

  AbcHandler handler(abc, &object_id_map);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"defgs\": {\"fire\": { \"d\": 30, \"e\": \"Enum1\" }, \"obj001\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";

  freeisle::json::test::check(result, expected);
}

TEST(MappedContainerLoader, TwoObjectsWithOccupiedIdMap) {
  const Abc abc = {
      .defgs = {Defg{.d = 30, .e = Enum1}, Defg{.d = 35, .e = Enum2}}};

  std::map<const void *, std::string> object_id_map = {
      {&abc.defgs[1], "obj001"},
  };

  AbcHandler handler(abc, &object_id_map);

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(handler, nullptr);

  const std::string expected =
      "{\"defgs\": {\"obj002\": { \"d\": 30, \"e\": \"Enum1\" }, \"obj001\": { "
      "\"d\": 35, \"e\": \"Enum2\" }}}";

  freeisle::json::test::check(result, expected);
}
