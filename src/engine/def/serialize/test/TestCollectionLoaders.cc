#include "def/serialize/CollectionLoaders.hh"

#include "def/Collection.hh"

#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "core/test/util/Util.hh"

#include <gtest/gtest.h>

namespace {

struct Object {
  std::string name;
};

struct Objects {
  freeisle::def::Collection<Object> objects;

  freeisle::def::NullableRef<Object> some_object;
  freeisle::def::RefMap<Object, uint32_t> object_numbers;
  freeisle::def::RefSet<Object> subset;
};

struct ObjectHandler {
  Object *obj;

  void set(Object &o) { obj = &o; }

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    obj->name = freeisle::json::loader::load<std::string>(ctx, value, "name");
  }
};

struct ObjectNumberHandler {
  uint32_t *n;

  void set(uint32_t &o) { n = &o; }

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    *n = freeisle::json::loader::load<uint32_t>(ctx, value, "number");
  }
};

struct ObjectsLoader {
  Objects &objects;

  void load(freeisle::json::loader::Context &ctx, Json::Value &value) {
    ObjectHandler object_loader;
    ObjectNumberHandler object_number_loader;

    freeisle::def::serialize::CollectionLoader<Object, ObjectHandler>
        objects_loader(objects.objects, object_loader);
    freeisle::def::serialize::RefMapLoader<Object, uint32_t,
                                           ObjectNumberHandler>
        refmap_loader(objects.object_numbers, objects.objects,
                      object_number_loader);

    freeisle::json::loader::load_object(ctx, value, "objects", objects_loader);
    objects.some_object = freeisle::def::serialize::load_ref(
        ctx, value, "some_object", objects.objects);
    freeisle::json::loader::load_object(ctx, value, "object_numbers",
                                        refmap_loader);
    objects.subset = freeisle::def::serialize::load_ref_set(
        ctx, value, "subset", objects.objects);
  }
};

} // namespace

TEST(CollectionLoader, Empty) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {}, \"object_numbers\": {}, \"subset\": []}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_EQ(objects.objects.size(), 0);
  EXPECT_TRUE(!objects.some_object);
  EXPECT_EQ(objects.object_numbers.size(), 0);
  ASSERT_EQ(objects.subset.size(), 0);
}

TEST(CollectionLoader, OneObject) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, \"some_object\": "
      "\"mine\", \"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": "
      "[]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_EQ(objects.objects.size(), 1);
  const freeisle::def::Collection<Object>::iterator iter =
      objects.objects.find("mine");
  ASSERT_NE(iter, objects.objects.end());
  EXPECT_EQ(iter->first, "mine");
  EXPECT_EQ(iter->second.name, "my object");

  EXPECT_EQ(&**objects.some_object, &iter->second);

  EXPECT_EQ(objects.object_numbers.size(), 1);
  const freeisle::def::RefMap<Object, uint32_t>::iterator iter2 =
      objects.object_numbers.find(iter);
  ASSERT_NE(iter2, objects.object_numbers.end());
  EXPECT_EQ(iter2->second, 55);

  ASSERT_EQ(objects.subset.size(), 0);
}

TEST(CollectionLoader, TwoObjects) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}, \"yours\": "
      "{\"name\": \"your object\"}}, \"some_object\": \"yours\", "
      "\"object_numbers\": {\"mine\": {\"number\": 55}, \"yours\": "
      "{\"number\": 33}}, \"subset\": [\"mine\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  freeisle::json::loader::load_root_object(data, loader);

  ASSERT_EQ(objects.objects.size(), 2);
  const freeisle::def::Collection<Object>::iterator mine_iter =
      objects.objects.find("mine");
  ASSERT_NE(mine_iter, objects.objects.end());
  const freeisle::def::Collection<Object>::iterator yours_iter =
      objects.objects.find("yours");
  ASSERT_NE(yours_iter, objects.objects.end());

  EXPECT_EQ(mine_iter->first, "mine");
  EXPECT_EQ(mine_iter->second.name, "my object");
  EXPECT_EQ(yours_iter->first, "yours");
  EXPECT_EQ(yours_iter->second.name, "your object");

  EXPECT_EQ(&**objects.some_object, &yours_iter->second);

  EXPECT_EQ(objects.object_numbers.size(), 2);
  const freeisle::def::RefMap<Object, uint32_t>::iterator mine_number_iter =
      objects.object_numbers.find(mine_iter);
  ASSERT_NE(mine_number_iter, objects.object_numbers.end());
  const freeisle::def::RefMap<Object, uint32_t>::iterator yours_number_iter =
      objects.object_numbers.find(yours_iter);
  ASSERT_NE(yours_number_iter, objects.object_numbers.end());
  EXPECT_EQ(mine_number_iter->second, 55);
  EXPECT_EQ(yours_number_iter->second, 33);

  ASSERT_EQ(objects.subset.size(), 1);
  EXPECT_EQ(objects.subset.count(mine_iter), 1);
  EXPECT_EQ(objects.subset.count(yours_iter), 0);
}

TEST(CollectionLoader, NonExistingObject) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, \"some_object\": "
      "\"none\", \"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": "
      "[]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Object with ID \"none\" does not exist");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 61);
    EXPECT_EQ(e.path(), "");
  }
}

TEST(CollectionLoader, RefSetMissing) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, "
      "\"object_numbers\": {\"mine\": {\"number\": 55}}}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Mandatory field \"subset\" is missing");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 1);
    EXPECT_EQ(e.path(), "");
  }
}

TEST(CollectionLoader, RefSetWrongType) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, "
      "\"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": 1}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Field \"subset\" is not of array type");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 100);
    EXPECT_EQ(e.path(), "");
  }
}

TEST(CollectionLoader, RefSetWrongRefType) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, "
      "\"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": [[]]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Type is not convertible to string");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 101);
    EXPECT_EQ(e.path(), "");
  }
}

TEST(CollectionLoader, RefSetNonexistingId) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text = "{\"objects\": {\"mine\": {\"name\": \"my "
                           "object\"}}, \"object_numbers\": {\"mine\": "
                           "{\"number\": 55}}, \"subset\": [\"None\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Object ID \"None\" does not exist in collection");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 101);
    EXPECT_EQ(e.path(), "");
  }
}

TEST(CollectionLoader, RefSetDuplicateId) {
  Objects objects{};
  ObjectsLoader loader{objects};

  const std::string text =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, "
      "\"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": "
      "[\"mine\", \"mine\"]}";
  std::vector<uint8_t> data(text.begin(), text.end());

  ASSERT_THROW_KEEP_AS_E(freeisle::json::loader::load_root_object(data, loader),
                         freeisle::json::loader::Error) {
    EXPECT_EQ(e.message(), "Duplicate object ID: \"mine\"");
    EXPECT_EQ(e.line(), 1);
    EXPECT_EQ(e.col(), 109);
    EXPECT_EQ(e.path(), "");
  }
}
