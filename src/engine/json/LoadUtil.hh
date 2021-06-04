#pragma once

#include "json/Loader.hh"

#include "core/Bitmask.hh"
#include "core/Enum.hh"
#include "core/EnumMap.hh"
#include "core/String.hh"

#include <fmt/format.h>

#include <cassert>

namespace freeisle::json::loader {

/**
 * Extract a primitive value from a json node as the given type, or throw
 * Json::Exception if the type does not match the expected type.
 */
template <typename T> inline T as(const Json::Value &value);
template <> inline uint32_t as(const Json::Value &val) { return val.asUInt(); }
template <> inline float as(const Json::Value &val) { return val.asFloat(); }
template <> inline bool as(const Json::Value &val) { return val.asBool(); }
template <> inline std::string as(const Json::Value &val) {
  return val.asString();
}

/**
 * Load a primitive value from a given key in an object value, throwing a
 * detailed loader::Error if either the type is not as expected or the value
 * is not present.
 */
template <typename T>
T load(Context &ctx, Json::Value &value, const char *key) {
  if (!value.isMember(key)) {
    throw Error::create(ctx, "", value,
                        fmt::format("Mandatory field \"{}\" is missing", key));
  }

  try {
    return as<T>(value[key]);
  } catch (const Json::Exception &ex) {
    throw Error::create(ctx, key, value[key], ex.what());
  }
}

/**
 * Load an enumerant value from a given key in an object value, throwing
 * a detailed loader::Error if the value is not present or not one of the
 * allowed enum values.
 */
template <typename T, uint32_t N>
T load_enum(Context &ctx, Json::Value &value, const char *key,
            const core::EnumEntry<T> (&entries)[N]) {
  const std::string str = load<std::string>(ctx, value, key);
  const T *val = freeisle::core::from_string(entries, str.c_str());

  if (val == nullptr) {
    const char *allowed[N];
    std::transform(std::begin(entries), std::end(entries), std::begin(allowed),
                   [](const core::EnumEntry<T> &t) { return t.str; });

    std::string message = fmt::format(
        "Illegal value: \"{}\". Allowed values are: {}", str,
        core::string::join(std::begin(allowed), std::end(allowed), ", "));
    throw freeisle::json::loader::Error::create(ctx, key, value[key],
                                                std::move(message));
  }

  return *val;
}

/**
 * Load a bitmask of enum values from the given key in the JSON file. The
 * format should be an array with one entry per bit set.
 */
template <typename T, uint32_t N>
core::Bitmask<T> load_bitmask(Context &ctx, Json::Value &value, const char *key,
                              const core::EnumEntry<T> (&entries)[N]) {
  const Json::Value &val = value[key];
  if (val.isNull()) {
    throw Error::create(ctx, "", value,
                        fmt::format("Mandatory field \"{}\" is missing", key));
  }

  if (!val.isArray()) {
    throw Error::create(ctx, key, val,
                        fmt::format("Field \"{}\" is not of array type", key));
  }

  core::Bitmask<T> result;
  for (uint32_t i = 0; i < val.size(); ++i) {
    std::string str;

    try {
      str = as<std::string>(val[i]);
    } catch (const Json::Exception &ex) {
      throw Error::create(ctx, key, val[i], ex.what());
    }

    const T *e = freeisle::core::from_string(entries, str.c_str());
    if (e == nullptr) {
      const char *allowed[N];
      std::transform(std::begin(entries), std::end(entries),
                     std::begin(allowed),
                     [](const core::EnumEntry<T> &t) { return t.str; });

      std::string message = fmt::format(
          "Illegal value: \"{}\". Allowed values are: {}", str,
          core::string::join(std::begin(allowed), std::end(allowed), ", "));

      throw freeisle::json::loader::Error::create(ctx, key, val[i],
                                                  std::move(message));
    }

    result |= *e;
  }

  return result;
}

/**
 * Load a byte sequence from the given key in an object value. If the context
 * has a file reference, the content of the value in the JSON document is
 * expected to be a reference to a file with the binary content, otherwise it
 * is interpreted as base64-encoded binary data.
 */
std::vector<uint8_t> load_binary(Context &ctx, Json::Value &value,
                                 const char *key);

/**
 * Load an object from a JSON object. The given handler is responsible for
 * loading the fields of the object from the JSON object. This function handles
 * include references and keeps tracking the tree walking in the context.
 */
template <typename THandler>
void load_object(Context &ctx, Json::Value &value, const char *key,
                 THandler &handler) {
  if (!value.isMember(key)) {
    throw Error::create(ctx, "", value,
                        fmt::format("Mandatory field \"{}\" is missing", key));
  }

  if (!value[key].isObject()) {
    throw Error::create(ctx, key, value[key],
                        "Expected value to be of object type");
  }

  const TreeDescent descent(ctx, key);

  resolve_includes(ctx, value[key]);

  handler.load(ctx, value[key]);
}

/**
 * A handler for containers where every entry has an ID that's not stored
 * in the object itself. The structure is something like this:
 *
 * `{"container": {"obj1": { ... }, "obj2": { ... }}`.
 *
 * The objects need to be default-constructible. Call the set() method before
 * loading the objects to set the container to be populated by the loader.
 */
template <typename ContainerT, typename ChildHandlerT>
class MappedContainerHandler {
public:
  /**
   * Construct a new mapped container handler. The given child handler
   * will be used to load each child object. The `set()` function is
   * called on the child handler before it is involked to load the child
   * object.
   */
  MappedContainerHandler(
      std::map<const void *, std::string> *object_ids = nullptr,
      ChildHandlerT child_handler = ChildHandlerT{})
      : child_handler_(child_handler), object_ids_(object_ids) {}

  /**
   * Load the container from the given json value. The value needs to be
   * of type object.
   */
  void load(Context &ctx, Json::Value &value) {
    assert(container_ != nullptr);

    container_->clear();
    index_.clear();

    // pass 1: populate container
    const Json::Value::Members &members = value.getMemberNames();
    for (const std::string &key : members) {
      container_->emplace_back();
      child_handler_.set(container_->back());
      loader::load_object(ctx, value, key.c_str(), child_handler_);
    }

    // pass 2: populate index. Separate pass in case
    // container iterators are invalidated when growing the container.
    typename ContainerT::iterator iter = container_->begin();
    for (const std::string &key : members) {
      assert(iter != container_->end());
      if (object_ids_) {
        (*object_ids_)[&*iter] = key;
      }
      index_[key] = iter;
      ++iter;
    }
  }

  /**
   * Set the container object to be populated.
   */
  void set(ContainerT &obj) { container_ = &obj; }

  /**
   * Look up an object by ID of the previosuly loaded container. Return
   * nullptr if no object with the given ID was loaded.
   */
  const typename ContainerT::value_type *lookup(const std::string &name) const {
    typename std::map<std::string,
                      typename ContainerT::iterator>::const_iterator iter =
        index_.find(name);
    if (iter == index_.end()) {
      return nullptr;
    }

    return &*iter->second;
  }

private:
  ContainerT *container_;
  std::map<std::string, typename ContainerT::iterator> index_;
  ChildHandlerT child_handler_;

  /**
   * A map of arbitrary loaded objects to IDs, so that the same IDs can be
   * re-used when saving the objects again, so that the include references
   * can be propagated during save. Needs to be kept up-to-date if the object
   * containers are modified between loading and saving.
   */
  std::map<const void *, std::string> *object_ids_;
};

/**
 * Loads an array of values of type T where each entry in the array
 * has an assigned name. The array is expected to be of object type, where
 * each key corresponds to the name of one of the array elements.
 */
template <typename T> class NamedArrayLoader {
public:
  /**
   * @param arr Pointer to the array where the values should be loaded into.
   * @param size Number of elements in the array.
   * @param names Names assigned to each array element. This names array
   *              should also have a length of the given size.
   */
  NamedArrayLoader(T *arr, size_t size, const char *const *names)
      : arr_(arr), size_(size), names_(names) {}

  /**
   * Load the enum array from given value of type object.
   */
  void load(Context &ctx, Json::Value &value) {
    for (size_t i = 0; i < size_; ++i) {
      arr_[i] = json::loader::load<T>(ctx, value, names_[i]);
    }
  }

private:
  T *arr_;
  const size_t size_;
  const char *const *const names_;
};

/**
 * Load an enum map as a named array (see NamedArrayLoader).
 */
template <typename T, typename... Enums> class EnumMapLoader {
public:
  /**
   * @param map The map to be loaded.
   * @param names Names assigned to each enum value. This can be generated
   *              with freeisle::core::get_enum_names().
   */
  EnumMapLoader(core::EnumMap<T, Enums...> &map,
                const core::EnumMap<const char *, Enums...> &names)
      : underlying(map.data(), map.size(), names.data()) {}

  /**
   * Load the enum array from the given value of type object.
   */
  void load(Context &ctx, Json::Value &value) { underlying.load(ctx, value); }

private:
  NamedArrayLoader<T> underlying;
};

} // namespace freeisle::json::loader
