#pragma once

#include <string>
#include <string_view>
#include <utility>

/**
 * Functions in fs::path manipulate filesystem paths. All functions
 * do so without accessing the filesystem, they only work on the
 * level of the paths.
 */
namespace freeisle::fs::path {

/**
 * Returns whether the given path is an absolute path or not.
 */
bool is_absolute(const std::string_view &path);

/**
 * Return the directory name of the given path, i.e. all directory
 * components except for the last component in the path. If the
 * path consists of only a single component, returns ".".
 */
std::string_view dirname(const std::string_view &path);

/**
 * Return the basename of the given path, i.e. the last component
 * in the path. If this is empty, then path is either the empty
 * string or the root directory "/". All other inputs will produce
 * a non-empty path. The result is guaranteed to never contain
 * the a '/' character.
 */
std::string_view basename(const std::string_view &path);

/**
 * Return the extension of the filename with the given path. The
 * extension is everything including and after the last "." in the
 * last component of the filename, or the full filename if there
 * is no ".".
 */
std::string_view extension(const std::string_view &path);

/**
 * Returns a pair of the directory name and the filename.
 */
std::pair<std::string_view, std::string_view>
split(const std::string_view &path);

/**
 * Resolves directory traversals such as "." and ".." in the given path.
 * Throws std::invalid_argument if the resolved path would end up on a higher
 * level than the given path, e.g. "/.." or "../". If the given path
 * is absolute, then the resolved path will be absolute as well, and if
 * the given path is a relative path, then the resolved path will be
 * relative as well.
 */
std::string resolve(const std::string_view &path);

/**
 * Join two path components with the directory separator. If the second
 * argument is absolute, then the first argument is ignored.
 */
std::string join(const std::string_view &front, const std::string_view &back);

/**
 * Tests whether the given path is a path relative to the given root, or not.
 * The paths are not resolved of directory traversals before checking, use
 * the resolve() function if that is necessary.
 */
bool is_relative_to(const std::string_view &path, const std::string_view &root);

/**
 * Make the given path relative to the given root path. If path is not
 * a path relative to root, throw std::invalid_argument.
 */
std::string_view make_relative(const std::string_view &path,
                               const std::string_view &root);

/**
 * Joins more than two path components.
 */
template <typename... Tail>
inline std::string join(const std::string_view &first,
                        const std::string_view &second, Tail &&... tail) {
  return join(join(first, second), std::forward<Tail>(tail)...);
}

} // namespace freeisle::fs::path
