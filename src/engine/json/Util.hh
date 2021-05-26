#pragma once

#include "json/Loader.hh"

#include "core/Enum.hh"
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

/**
 * Load an enumerant value from a given key in an object value, throwing
 * a detailed loader::Error if the value is not present or not one of the
 * allowed enum values.
 */
template <typename T, uint32_t N>
T load_enum(freeisle::json::loader::Context &ctx, Json::Value &value,
            const char *key, const core::EnumEntry<T> (&entries)[N]) {
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
  MappedContainerHandler(ChildHandlerT child_handler = ChildHandlerT{})
      : child_handler_(child_handler) {}

  /**
   * Load the container from the given json value. The value needs to be
   * of type object.
   */
  void load(loader::Context &ctx, Json::Value &value) {
    assert(container_ != nullptr);

    container_->clear();
    index_.clear();

    // pass 1: populate container
    const Json::Value::Members &members = value.getMemberNames();
    for (const std::string &key : members) {
      container_->emplace_back();
      child_handler_.set(container_->back());
      loader::load_object(ctx, key.c_str(), value[key], child_handler_);
    }

    // pass 2: populate index. Separate pass in case
    // container iterators are invalidated when growing the container.
    typename ContainerT::iterator iter = container_->begin();
    for (const std::string &key : members) {
      assert(iter != container_->end());
      object_ids_[&*iter] = key;
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
  std::map<const void *, std::string> object_ids_;
};

} // namespace freeisle::json::loader
