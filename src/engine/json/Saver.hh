#pragma once

#include <json/json.h>

#include "fs/File.hh"
#include "json/IncludeInfo.hh"

namespace freeisle::json::saver {

/**
 * Save context, keeps track of the current state as the object tree is
 * being walked.
 */
struct Context {
  Context(const Context &) = delete;
  Context(Context &&) = default;

  Context &operator=(const Context &) = delete;
  Context &operator=(Context &&) = default;

  /**
   * Path in the filesystem at which the document is being saved.
   */
  const std::string path;

  /**
   * Location in the object tree in the form ".a.b.c" where the object
   * currently being saved is located.
   */
  std::string current_location;

  /**
   * Mapping from paths in the object tree in the form of ".a.b.c" to
   * include information. This is used to place include references to other
   * files instead of saving everything in the main file.
   */
  const std::map<std::string, IncludeInfo> &include_map;
};

/**
 * Changes the currently active path in the JSON node tree in the given
 * context. This is used internally to track where in the tree we are,
 * also when resolving includes.
 * Restores the context to the previous state on destruction.
 */
class TreeLocationChange {
public:
  /**
   * @param ctx The context to adjust.
   * @param key The key in the object tree that was walked down.
   */
  TreeLocationChange(Context &ctx, const std::string &key);
  ~TreeLocationChange();

  TreeLocationChange(const TreeLocationChange &) = delete;
  TreeLocationChange(TreeLocationChange &&) = delete;

  TreeLocationChange &operator=(const TreeLocationChange &) = delete;
  TreeLocationChange &operator=(TreeLocationChange &&) = delete;

private:
  Context &ctx_;
  const std::string location_;
};

/**
 * Replaces all or parts of the information in the given JSON value with
 * an include reference if one is specified in the context's include map.
 */
void restore_includes(const Context &ctx, Json::Value &value);

/**
 * Main entry point to the saver for saving to an in-memory JSON
 * representation.
 */
template <typename THandler>
std::vector<uint8_t>
save_root_object(THandler &handler,
                 const std::map<std::string, IncludeInfo> *include_map) {
  const std::map<std::string, IncludeInfo> empty_include_map;
  if (include_map == nullptr) {
    include_map = &empty_include_map;
  }

  Context ctx{
      .path = "",
      .current_location = "",
      .include_map = *include_map,
  };

  Json::Value root;
  handler.save(ctx, root);
  restore_includes(ctx, root);

  Json::StyledWriter writer;
  std::string str = writer.write(root);
  return std::vector<uint8_t>(str.begin(), str.end());
}

/**
 * Main entry point to the saver for saving into a file in JSON
 * representation.
 */
template <typename THandler>
void save_root_object(const char *path, THandler &handler,
                      const std::map<std::string, IncludeInfo> *include_map) {
  const std::map<std::string, IncludeInfo> empty_include_map;
  if (include_map == nullptr) {
    include_map = &empty_include_map;
  }

  Context ctx{
      .path = path,
      .current_location = "",
      .include_map = *include_map,
  };

  Json::Value root;
  handler.save(ctx, root);
  restore_includes(ctx, root);

  Json::StyledWriter writer;
  const std::string str = writer.write(root);
  fs::write_file(path, reinterpret_cast<const uint8_t *>(str.data()),
                 str.size(), nullptr);
}

} // namespace freeisle::json::saver
