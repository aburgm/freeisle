#pragma once

#include "def/Collection.hh"

#include "json/SaveUtil.hh"
#include "json/Saver.hh"

namespace freeisle::state::serialize {

/**
 * Saves a collection in the JSON document as an object with one entry for
 * each item in the collection. The object ID is used as the key in the JSON
 * object.
 */
template <typename T, typename ChildHandlerT> class CollectionSaver {
public:
  /**
   * Construct a new collection saver. The given child handler
   * will be used to save each child object. The `set()` function is
   * called on the child handler before it is invoked to save a child
   * object.
   */
  CollectionSaver(const def::Collection<T> &collection,
                  ChildHandlerT child_handler = ChildHandlerT{})
      : collection_(&collection), child_handler_(child_handler) {}

  /**
   * Save the container to the given json value.
   */
  void save(json::saver::Context &ctx, Json::Value &value) {
    assert(collection_ != nullptr);

    for (typename def::Collection<T>::const_iterator iter =
             collection_->begin();
         iter != collection_->end(); ++iter) {
      child_handler_.set(iter->second);
      json::saver::save_object(ctx, value, iter->first.c_str(), child_handler_);
    }
  }

private:
  const def::Collection<T> *collection_;
  ChildHandlerT child_handler_;
};

/**
 * Save an object reference in the JSON document. It is saved as a value of
 * type string containing the object ID.
 */
template <typename T>
void save_ref(json::saver::Context &ctx, Json::Value &value, const char *key,
              const def::NullableRef<T> &ref,
              const def::Collection<T> &collection) {
  if (ref) {
    json::saver::save(ctx, value, key, ref->id());
  }
}

/**
 * Save a reference map as an object in a JSON document. The object IDs of
 * the underlying collection are used the keys of the JSON object. There must
 * be one entry in the RefMap for each entry in the underlying collection.
 */
template <typename C, typename T, typename ChildHandlerT> class RefMapSaver {
public:
  RefMapSaver(const def::RefMap<C, T> &map,
              const def::Collection<C> &collection,
              ChildHandlerT child_handler = ChildHandlerT{})
      : map_(&map), collection_(collection), child_handler_(child_handler) {}

  void save(json::saver::Context &ctx, Json::Value &value) {
    assert(map_ != nullptr);

    // RefMap should have one entry for each item in the collection: iterate
    // through collection to verify that.
    for (typename def::Collection<C>::const_iterator iter = collection_.begin();
         iter != collection_.end(); ++iter) {
      const typename def::RefMap<C, T>::const_iterator map_iter =
          map_->find(iter);
      assert(map_iter != map_->end());

      child_handler_.set(map_iter->second);
      json::saver::save_object(ctx, value, map_iter->first.id().c_str(),
                               child_handler_);
    }
  }

private:
  const def::RefMap<C, T> *map_;
  const def::Collection<C> &collection_;
  ChildHandlerT child_handler_;
};

/**
 * Save a reference set in the JSON document. It is saved as an array with
 * string values, each denoting the object ID of an item in the set.
 */
template <typename T>
void save_ref_set(json::saver::Context &ctx, Json::Value &value,
                  const char *key, const def::RefSet<T> &set,
                  const def::Collection<T> &collection) {
  value[key] = Json::Value(Json::ValueType::arrayValue);
  Json::Value &val = value[key];

  for (const def::Ref<T> &ref : set) {
    val.append(ref.id());
  }
}

} // namespace freeisle::state::serialize
