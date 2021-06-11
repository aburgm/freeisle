#include "def/serialize/CollectionSavers.hh"

#include "def/Collection.hh"

#include "json/SaveUtil.hh"
#include "json/Saver.hh"

#include "json/test/Util.hh"

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
  const Object *obj;

  void set(freeisle::def::Ref<const Object> o) { obj = &*o; }

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save(ctx, value, "name", obj->name);
  }
};

struct ObjectNumberHandler {
  uint32_t n;

  void set(uint32_t o) { n = o; }

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    freeisle::json::saver::save(ctx, value, "number", n);
  }
};

struct ObjectsSaver {
  const Objects &objects;

  void save(freeisle::json::saver::Context &ctx, Json::Value &value) {
    ObjectHandler object_saver;
    ObjectNumberHandler object_number_saver;

    freeisle::def::serialize::CollectionSaver<Object, ObjectHandler>
        objects_saver(objects.objects, object_saver);
    freeisle::def::serialize::RefMapSaver<Object, uint32_t, ObjectNumberHandler>
        refmap_saver(objects.object_numbers, objects.objects,
                     object_number_saver);

    freeisle::json::saver::save_object(ctx, value, "objects", objects_saver);
    freeisle::def::serialize::save_ref(ctx, value, "some_object",
                                       objects.some_object, objects.objects);
    freeisle::json::saver::save_object(ctx, value, "object_numbers",
                                       refmap_saver);
    freeisle::def::serialize::save_ref_set(ctx, value, "subset", objects.subset,
                                           objects.objects);
  }
};

} // namespace

TEST(CollectionSaver, Empty) {
  Objects objects{};
  ObjectsSaver saver{objects};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);

  const std::string expected_value =
      "{\"objects\": {}, \"object_numbers\": {}, \"subset\": []}";
  freeisle::json::test::check(result, expected_value);
}

TEST(CollectionSaver, OneObject) {
  Objects objects{
      .objects =
          {
              {"mine", {.name = "my object"}},
          },
  };

  freeisle::def::Collection<Object>::iterator iter =
      objects.objects.find("mine");

  objects.some_object = iter;
  objects.object_numbers[iter] = 55;

  ObjectsSaver saver{objects};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);

  const std::string expected_value =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}}, \"some_object\": "
      "\"mine\", \"object_numbers\": {\"mine\": {\"number\": 55}}, \"subset\": "
      "[]}";
  freeisle::json::test::check(result, expected_value);
}

TEST(CollectionSaver, TwoObjects) {
  Objects objects{
      .objects =
          {
              {"mine", {.name = "my object"}},
              {"yours", {.name = "your object"}},
          },
  };

  freeisle::def::Collection<Object>::iterator mine_iter =
      objects.objects.find("mine");
  freeisle::def::Collection<Object>::iterator yours_iter =
      objects.objects.find("yours");

  objects.some_object = yours_iter;
  objects.object_numbers[mine_iter] = 55;
  objects.object_numbers[yours_iter] = 33;
  objects.subset.insert(mine_iter);

  ObjectsSaver saver{objects};

  const std::vector<uint8_t> result =
      freeisle::json::saver::save_root_object(saver, nullptr);

  const std::string expected_value =
      "{\"objects\": {\"mine\": {\"name\": \"my object\"}, \"yours\": "
      "{\"name\": \"your object\"}}, \"some_object\": \"yours\", "
      "\"object_numbers\": {\"mine\": {\"number\": 55}, \"yours\": "
      "{\"number\": 33}}, \"subset\": [\"mine\"]}";
  freeisle::json::test::check(result, expected_value);
}
