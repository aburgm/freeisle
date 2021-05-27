#pragma once

#include "json/Saver.hh"

#include "core/Enum.hh"

#include <fmt/format.h>

#include <cassert>
#include <set>

namespace freeisle::json::saver {

/**
 * Save a primitive value in a JSON object under a given key.
 */
template <typename T>
void save(Context &ctx, Json::Value &value, const char *key, T val) {
  value[key] = std::move(val);
}

/**
 * Save an enumerant value in a JSON object under a given key.
 */
template <typename T, uint32_t N>
void save_enum(Context &ctx, Json::Value &value, const char *key, T val,
               const core::EnumEntry<T> (&entries)[N]) {
  const char *str = freeisle::core::to_string(entries, val);
  assert(str != nullptr);

  save(ctx, value, key, str);
}

/**
 * Save an object handled by the given handler under the given key in the
 * value provided. Handles include reference restoration.
 */
template <typename THandler>
void save_object(Context &ctx, Json::Value &value, const char *key,
                 THandler &handler) {
  const TreeLocationChange tc(ctx, key);
  const std::map<std::string, IncludeInfo>::const_iterator iter =
      ctx.include_map.find(ctx.current_location);

  // This check is just a fast path to not even run the handler if not needed:
  if (iter != ctx.include_map.end() && iter->second.override_keys.empty()) {
    if (!iter->second.filename.empty()) {
      value[key]["include"] = iter->second.filename;
    }

    // no override keys, can skip serialization of sub-object altogether
    return;
  }

  value[key] = Json::Value(Json::ValueType::objectValue);
  Json::Value &obj = value[key];
  handler.save(ctx, obj);

  restore_includes(ctx, obj);
}

/**
 * A handler for containers where every entry has an ID that's not stored
 * in the object itself. The structure is something like this:
 *
 * `{"container": {"obj1": { ... }, "obj2": { ... }}`.
 *
 * Call the set() method before saving objects to set the container being
 * saved.
 */
template <typename ContainerT, typename ChildHandlerT>
class MappedContainerHandler {
public:
  /**
   * Construct a new mapped container handler. The given child handler
   * will be used to load each child object. The `set()` function is
   * called on the child handler before it is involked to load the child
   * object.
   *
   * Set object_ids if you want the saver to use pre-defined object IDs.
   * This is especially useful for placing include references correctly
   * with those objects. The map is also updated with any new objects that
   * don't have an object ID yet, in which case they are assigned one.
   */
  MappedContainerHandler(
      std::map<const void *, std::string> *object_ids = nullptr,
      ChildHandlerT child_handler = ChildHandlerT{})
      : child_handler_(child_handler), object_ids_(object_ids) {}

  /**
   * Save the container to the given json value. The value is of type object.
   */
  void save(Context &ctx, Json::Value &value) {
    assert(container_ != nullptr);

    std::set<std::string> known_ids;
    if (object_ids_) {
      for (const typename ContainerT::value_type &val : *container_) {
        const std::map<const void *, std::string>::const_iterator iter =
            object_ids_->find(&val);
        if (iter != object_ids_->end()) {
          known_ids.insert(iter->second);
        }
      }
    }

    uint32_t n = 0;
    for (const typename ContainerT::value_type &val : *container_) {
      std::string key;
      if (object_ids_) {
        const std::map<const void *, std::string>::const_iterator iter =
            object_ids_->find(&val);
        if (iter != object_ids_->end()) {
          key = iter->second;
        }
      }

      if (key.empty()) {
        do {
          ++n;
          key = fmt::format("obj{:03}", n);
        } while (known_ids.find(key) != known_ids.end());

        if (object_ids_) {
          (*object_ids_)[&val] = key;
        }
      }

      child_handler_.set(val);
      save_object(ctx, value, key.c_str(), child_handler_);
    }
  }

  /**
   * Set the container object to be populated.
   */
  void set(const ContainerT &obj) { container_ = &obj; }

private:
  const ContainerT *container_;
  ChildHandlerT child_handler_;
  std::map<const void *, std::string> *object_ids_;
};

} // namespace freeisle::json::saver
