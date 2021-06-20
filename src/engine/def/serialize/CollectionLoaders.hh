#pragma once

#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "def/Collection.hh"

namespace freeisle::def::serialize {

/**
 * Loads a def::Collection of objects of type T. It expects a JSON object
 * where the keys of the object are used as object IDs in the collection.
 */
template <typename T, typename ChildHandlerT> class CollectionLoader {
public:
  /**
   * Construct a new collection loader. The given child handler
   * will be used to load each child object. The `set()` function is
   * called on the child handler before it is invoked to load a child
   * object.
   */
  CollectionLoader(def::Collection<T> &collection,
                   ChildHandlerT child_handler = ChildHandlerT{})
      : collection_(&collection), child_handler_(child_handler) {}

  /**
   * Load the container from the given json value. The value needs to be
   * of type object.
   */
  void load(json::loader::Context &ctx, Json::Value &value) {
    assert(collection_ != nullptr);
    collection_->clear();

    const Json::Value::Members &members = value.getMemberNames();
    for (const std::string &key : members) {
      const std::pair<typename def::Collection<T>::iterator, bool> result =
          collection_->try_emplace(key);
      assert(result.second);

      child_handler_.set(result.first);
      json::loader::load_object(ctx, value, key.c_str(), child_handler_);
    }
  }

private:
  def::Collection<T> *collection_;
  ChildHandlerT child_handler_;
};

/**
 * A simple child handler which does not populate the child objects, if
 * in conjunction with a CollectionLoader, it will cause the collection to
 * be populated with empty objects.
 */
template <typename T> class EmptyChildHandler {
public:
  void set(def::Ref<T> obj) {}
  void load(json::loader::Context &ctx, Json::Value &value) {}
};

/**
 * A shortcut for a CollectionLoader with an empty child handler. This can
 * be useful when loading the actual information into the objects later
 * with CollectionLoaderPass. It can be used to load objects with cyclic
 * references.
 */
template <typename T>
using EmptyCollectionLoader = CollectionLoader<T, EmptyChildHandler<T>>;

/**
 * For a given def::Collection of type T, loads additional information from
 * a JSON object which is expected to contain a subset of the objects present
 * in the collection.
 */
template <typename T, typename ChildHandlerT> class CollectionLoaderPass {
public:
  CollectionLoaderPass(def::Collection<T> &collection,
                       ChildHandlerT child_handler = ChildHandlerT{})
      : collection_(&collection), child_handler_(child_handler) {}

  void load(json::loader::Context &ctx, Json::Value &value) {
    for (typename def::Collection<T>::iterator iter = collection_->begin();
         iter != collection_->end(); ++iter) {
      child_handler_.set(iter);
      json::loader::load_object(ctx, value, iter->first.c_str(),
                                child_handler_);
    }
  }

private:
  def::Collection<T> *collection_;
  ChildHandlerT child_handler_;
};

/**
 * Loads an object reference from a JSON object. In the JSON document, the
 * value with the given key is expected to be a string denoting the object ID.
 */
template <typename T>
def::Ref<T> load_mandatory_ref(json::loader::Context &ctx, Json::Value &value,
                               const char *key,
                               def::Collection<T> &collection) {
  const std::string str = json::loader::load<std::string>(ctx, value, key);
  const typename def::Collection<T>::iterator iter = collection.find(str);
  if (iter == collection.end()) {
    throw json::loader::Error::create(
        ctx, key, value[key],
        fmt::format("Object with ID \"{}\" does not exist", str));
  }

  return iter;
}

/**
 * Loads an object reference from a JSON object. In the JSON document, the
 * value with the given key is expected to be a string denoting the object ID.
 * If there is no key with the given ID in the object, a null Ref is returned.
 */
template <typename T>
def::NullableRef<T> load_nullable_ref(json::loader::Context &ctx,
                                      Json::Value &value, const char *key,
                                      def::Collection<T> &collection) {
  if (!value.isMember(key)) {
    return def::NullableRef<T>{};
  }

  return load_mandatory_ref(ctx, value, key, collection);
}

/**
 * Loads a reference map from a JSON object. The JSON object is expected to
 * have the same keys as the object IDs of the underlying collection.
 */
template <typename C, typename T, typename ChildHandlerT> class RefMapLoader {
public:
  RefMapLoader(def::RefMap<C, T> &map, def::Collection<C> &collection,
               ChildHandlerT child_handler = ChildHandlerT{})
      : map_(&map), collection_(collection), child_handler_(child_handler) {}

  void load(json::loader::Context &ctx, Json::Value &value) {
    assert(map_ != nullptr);
    map_->clear();

    for (typename def::Collection<C>::iterator iter = collection_.begin();
         iter != collection_.end(); ++iter) {
      const std::pair<typename def::RefMap<C, T>::iterator, bool> result =
          map_->try_emplace(iter);
      assert(result.second);

      child_handler_.set(iter, result.first->second);
      json::loader::load_object(ctx, value, iter->first.c_str(),
                                child_handler_);
    }
  }

private:
  def::RefMap<C, T> *map_;
  def::Collection<C> &collection_;
  ChildHandlerT child_handler_;
};

/**
 * Load a reference set from a JSON object. In the JSON document, the value
 * with the given key is expected to be of array type, with one string entry
 * for each item in the set.
 */
template <typename T>
def::RefSet<T> load_ref_set(json::loader::Context &ctx, Json::Value &value,
                            const char *key, def::Collection<T> &collection) {
  if (!value.isMember(key)) {
    throw json::loader::Error::create(
        ctx, "", value, fmt::format("Mandatory field \"{}\" is missing", key));
  }

  const Json::Value &val = value[key];
  if (!val.isArray()) {
    throw json::loader::Error::create(
        ctx, key, val, fmt::format("Field \"{}\" is not of array type", key));
  }

  def::RefSet<T> result;
  for (uint32_t i = 0; i < val.size(); ++i) {
    std::string str;

    try {
      str = json::loader::as<std::string>(val[i]);
    } catch (const Json::Exception &ex) {
      throw json::loader::Error::create(ctx, key, val[i], ex.what());
    }

    const typename def::Collection<T>::iterator iter = collection.find(str);
    if (iter == collection.end()) {
      throw json::loader::Error::create(
          ctx, key, val[i],
          fmt::format("Object ID \"{}\" does not exist in collection", str));
    }

    const std::pair<typename def::RefSet<T>::iterator, bool> inserted =
        result.insert(iter);
    if (!inserted.second) {
      throw json::loader::Error::create(
          ctx, key, val[i], fmt::format("Duplicate object ID: \"{}\"", str));
    }
  }

  return result;
}

} // namespace freeisle::def::serialize
