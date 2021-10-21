#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace freeisle::core::string {

/**
 * Returns whether the string str starts with the given prefix.
 */
bool has_prefix(const std::string_view &str, const std::string_view &prefix);

/**
 * Returns whether the string str ends with the given suffix.
 */
bool has_suffix(const std::string_view &str, const std::string_view &suffix);

/**
 * Splits a string at every occurrence of the given separator.
 */
std::vector<std::string_view> split(const std::string_view &str,
                                    const std::string_view &sep);

/**
 * Joins a sequence of strings to a resulting string, each element separated
 * by the given separator.
 */
template <typename IteratorT>
std::string join(IteratorT begin, IteratorT end, const std::string_view &sep) {
  if (begin == end) {
    return std::string();
  }

  std::string result(*begin++);
  for (; begin < end; ++begin) {
    result += sep;
    result += *begin;
  }

  return result;
}

} // namespace freeisle::core::string
