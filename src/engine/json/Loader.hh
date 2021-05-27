#pragma once

#include <json/json.h>

#include "fs/FileInfo.hh"
#include "json/IncludeInfo.hh"

#include <cstdint>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace freeisle::json::loader {

/**
 * Informaton about where an object or part of an object is loaded from.
 */
struct SourceInfo {
  /**
   * Name of the file which is being loaded.
   */
  std::string filename;

  /**
   * Absolute path on the filesystem to the file being loaded.
   */
  std::string path;

  /**
   * Unique file ID, used for cyclic include detection.
   */
  fs::FileId id;

  /**
   * Level in the search path where the file was found. Includes are
   * resolved only in this level and lower levels, but not higher levels.
   */
  uint32_t level;

  /**
   * Source from which this source was included, or nullptr if it is at the
   * root of the include tree.
   */
  const SourceInfo *origin;

  /**
   * Source text. This is only used for looking up line/column information
   * for error messages which is unfortunately not available in the json::Value.
   */
  const std::vector<uint8_t> source_data;
};

/**
 * Information about the origin of an object node in the JSON document.
 */
struct OriginInfo {
  /**
   * A map of key names to where the corresponding key was loaded from.
   * For simple keys, i.e. primitive types or lists, this is always set if
   * the key was loaded from a different file via an include. It is not set
   * if the key was loaded from the main file.
   *
   * For object types, this is only set if the whole object was loaded from the
   * given source. If some keys of the include were defined in the base main
   * file, there will not be an entry here, but there will be entries for the
   * keys from the included object in the corresponding children's OriginInfo.
   */
  std::map<std::string, const SourceInfo *> included_from;
};

/**
 * Loading context. This is transient state used by the loader while loading
 * an object. Part of the context can be used/preserved and used by the saver
 * later in order to save back references to included files instead of dumping
 * all objects in a single JSON object.
 */
struct Context {
  Context(const Context &) = delete;
  Context(Context &&) = default;

  Context &operator=(const Context &) = delete;
  Context &operator=(Context &&) = default;

  /**
   * Paths is which to search for include files, in order of priority.
   */
  const std::vector<std::string> search_paths;

  /**
   * List of source files from which data was loaded.
   */
  std::list<SourceInfo> sources;

  /**
   * Pointer to the source from which the object currently being loaded
   * is loaded from.
   */
  const SourceInfo *current_source;

  /**
   * Location in the object tree in the form ".a.b.c" where the object
   * currently being loaded is loaded from.
   */
  std::string current_location;

  /**
   * Mapping from paths in the object tree in the form of ".a.b.c" to
   * the source information of where it was loaded from. If for a given
   * tree path there is no entry in the include_map, then the path
   * from the parent applies.
   */
  std::map<std::string, OriginInfo> origin_map;

  /**
   * Mapping from paths in the object tree in the form of ".a.b.c" to
   * include information. Any include reference that is resolved in the
   * main file wil get one entry in the include map.
   */
  std::map<std::string, IncludeInfo> include_map;
};

/**
 * An exception class for an error that happened during the loading process.
 * Contains references to where exactly the loading error happened.
 */
class Error : public std::runtime_error {
public:
  /**
   * Create a new error object that can be thrown.
   * @param ctx The current loading context at the time the error is
   * encountered.
   * @param key If not empty, the key under the context's current location where
   * the error was encountered.
   * @param val The JSON value that led to the error. Line number information is
   *            extracted from it.
   * @param message A human-readable error message.
   */
  static Error create(const Context &ctx, const std::string &key,
                      const Json::Value &val, std::string message);

  /**
   * Returns the text of the error message.
   */
  const std::string &message() const;

  /**
   * Returns the full absolute path to the file that was loaded when the
   * error occurred.
   */
  const std::string &path() const;

  /**
   * Returns the line in the file there the error occurred.
   */
  uint32_t line() const;

  /**
   * Returns the column in the file there the error occurred.
   */
  uint32_t col() const;

private:
  Error(std::string formatted_message, std::string message, std::string path,
        uint32_t line, uint32_t col);

  const std::string message_;
  const std::string path_;
  const uint32_t line_;
  const uint32_t col_;
};

/**
 * Changes the currently active source file in the context.
 * Typically used internally when include files are resolved.
 * Restores the context to the previous state on destruction.
 */
class TreeSourceChange {
public:
  /**
   * @param ctx The context to change the current source on.
   * @param source The new source to set.
   */
  TreeSourceChange(Context &ctx, const SourceInfo &source);
  ~TreeSourceChange();

  TreeSourceChange(const TreeSourceChange &) = delete;
  TreeSourceChange(TreeSourceChange &&) = delete;

  TreeSourceChange &operator=(const TreeSourceChange &) = delete;
  TreeSourceChange &operator=(TreeSourceChange &&) = delete;

private:
  Context &ctx_;
  const SourceInfo &source_;
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
 * Adjusts the loading context to represent a descent in the tree, and on
 * destruction restores the context to the previous state. The effect of this
 * is a combination of (a possible) TreeSourceChange, and a
 * TreeLocationChange.
 */
class TreeDescent {
public:
  /**
   * @param ctx The context to adjust.
   * @param key The key in the object tree that was walked down.
   */
  TreeDescent(Context &ctx, const std::string &key);

private:
  const TreeSourceChange source_;
  const TreeLocationChange location_;
};

/**
 * Resolves include references in the given JSON object, i.e. if the
 * given JSON object has a key named "include", it opens the corresponding
 * file and merges all the keys in the top-level object in that file into
 * the given JSON object.
 *
 * The function works recursively if the included file has another include
 * reference.
 *
 * Uses context to determine the search paths to resolve include references,
 * and to catch cyclic include references.
 */
void resolve_includes(Context &ctx, Json::Value &value);

/**
 * Create a new loading context, not related to any files. It will not be
 * able to resolve any include references.
 *
 * Use load_object subsequently to use the context to load an object.
 */
std::pair<Context, Json::Value>
make_root_source_context(std::vector<uint8_t> data);

/**
 * Create a new loading context from a given root path on the filesystem.
 * The JSON document at this location will be parsed and returned together
 * with the context. If objects aro loaded from this JSON object, then
 * include references will be resolved relative to this root path.
 */
std::pair<Context, Json::Value> make_root_file_context(const char *path);

/**
 * Main entry point to the loader for loading an in-memory JSON
 * representation. In this form, include references are not resolved.
 */
template <typename THandler>
std::map<std::string, IncludeInfo> load_root_object(std::vector<uint8_t> data,
                                                    THandler &handler) {
  std::pair<Context, Json::Value> pair =
      make_root_source_context(std::move(data));
  resolve_includes(pair.first, pair.second);
  handler.load(pair.first, pair.second);
  return std::move(pair.first.include_map);
}

/**
 * Main entry point to the loader for loading a file-backed JSON
 * representation. Include references are resolved relative to the
 * file path.
 */
template <typename THandler>
std::map<std::string, IncludeInfo> load_root_object(const char *path,
                                                    THandler &handler) {
  std::pair<Context, Json::Value> pair = make_root_file_context(path);
  resolve_includes(pair.first, pair.second);
  handler.load(pair.first, pair.second);
  return std::move(pair.first.include_map);
}

} // namespace freeisle::json::loader
