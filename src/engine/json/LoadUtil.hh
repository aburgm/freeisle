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
template <> inline uint8_t as(const Json::Value &val) { return val.asUInt(); }
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
  if (!value.isMember(key)) {
    throw Error::create(ctx, "", value,
                        fmt::format("Mandatory field \"{}\" is missing", key));
  }

  const Json::Value &val = value[key];
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
