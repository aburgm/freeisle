#pragma once

#include <string>
#include <vector>

namespace freeisle::core::string {

/**
 * Returns whether the string str starts with the given prefix.
 */
bool has_prefix(const std::string &str, const std::string &prefix);

/**
 * Returns whether the string str ends with the given suffix.
 */
bool has_suffix(const std::string &str, const std::string &suffix);

/**
 * Splits a string at every occurrence of the given separator.
 */
std::vector<std::string> split(const std::string &str, const std::string &sep);

/**
 * Joins a sequence of strings to a resulting string, each element separated
 * by the given separator.
 */
template <typename IteratorT>
std::string join(IteratorT begin, IteratorT end, const std::string &sep) {
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
