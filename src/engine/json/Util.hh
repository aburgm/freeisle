#pragma once

#include "json/Loader.hh"

#include <fmt/format.h>

namespace freeisle::json::loader {

/**
 * Extract a primitive value from a json node as the given type, or throw
 * Json::Exception if the type does not match the expected type.
 */
template <typename T> T as(const Json::Value &value);
template <> uint32_t as(const Json::Value &val) { return val.asUInt(); }
template <> float as(const Json::Value &val) { return val.asFloat(); }
template <> bool as(const Json::Value &val) { return val.asBool(); }
template <> std::string as(const Json::Value &val) { return val.asString(); }

/**
 * Load a primitive value from a given key in an object value, throwing a
 * detailed loader::Error if either the type is not as expected or the value
 * is not present.
 */
template <typename T>
T load(freeisle::json::loader::Context &ctx, Json::Value &value,
       const char *key) {
  if (!value.isMember(key)) {
    throw freeisle::json::loader::Error::create(
        ctx, "", value, fmt::format("Mandatory field \"{}\" is missing", key));
  }

  try {
    return as<T>(value[key]);
  } catch (const Json::Exception &ex) {
    throw freeisle::json::loader::Error::create(ctx, key, value[key],
                                                ex.what());
  }
}

} // namespace freeisle::json::loader
