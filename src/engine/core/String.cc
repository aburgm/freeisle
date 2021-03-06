#include "core/String.hh"

#include <cassert>

namespace freeisle::core::string {

bool has_prefix(const std::string_view &str, const std::string_view &prefix) {
  return str.length() >= prefix.length() &&
         str.compare(0, prefix.length(), prefix) == 0;
}

bool has_suffix(const std::string_view &str, const std::string_view &suffix) {
  return str.length() >= suffix.length() &&
         str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
             0;
}

std::vector<std::string_view> split(const std::string_view &str,
                                    const std::string_view &sep) {
  assert(!sep.empty());

  std::vector<std::string_view> result;

  std::string::size_type prev = 0;
  std::string::size_type pos;
  while ((pos = str.find(sep, prev)) != std::string::npos) {
    result.push_back(str.substr(prev, pos - prev));
    prev = pos + sep.length();
  }

  result.push_back(str.substr(prev));
  return result;
}

} // namespace freeisle::core::string
