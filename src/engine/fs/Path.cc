#include "fs/Path.hh"

#include "core/String.hh"

#include <fmt/format.h>

#include <cassert>
#include <stdexcept>
#include <vector>

/**
 * This implements the filesystem path manipulators assuming a
 * POSIX filesystem. Different filesystems will need different
 * implementations.
 */

namespace freeisle::fs::path {

bool is_absolute(const std::string_view &path) {
  return !path.empty() && path[0] == '/';
}

std::string dirname(const std::string &path) {
  const std::pair<std::string, std::string> pair = split(path);
  return pair.first;
}

std::string basename(const std::string &path) {
  const std::pair<std::string, std::string> pair = split(path);
  return pair.second;
}

const std::string extension(const std::string &path) {
  const std::string bn = basename(path);
  const std::string::size_type pos = bn.rfind('.');

  if (pos == std::string::npos) {
    return std::string{};
  } else {
    return bn.substr(pos);
  }
}

std::pair<std::string, std::string> split(const std::string &path) {
  if (path.empty()) {
    return {".", ""};
  }

  const std::string::size_type len = path.find_last_not_of("/");
  if (len == std::string::npos) {
    return {"/", ""};
  }

  const std::string::size_type pos = path.rfind('/', len);
  if (pos == std::string::npos) {
    return {".", path.substr(0, len + 1)};
  } else if (pos == 0) {
    return {"/", path.substr(1, len)};
  } else {
    return {path.substr(0, pos), path.substr(pos + 1, len - pos)};
  }
}

std::string resolve(const std::string_view &path) {
  std::vector<std::string> parts;
  std::string::size_type prev = 0;
  while (prev != std::string::npos) {
    std::string::size_type pos = path.find('/', prev);

    std::string component;
    if (pos != std::string::npos) {
      component = path.substr(prev, pos - prev);
      prev = pos + 1;
    } else {
      component = path.substr(prev);
      prev = pos;
    }

    if (component.empty() || component == ".") {
      continue;
    } else if (component == "..") {
      if (parts.empty()) {
        throw std::invalid_argument(
            fmt::format("\"{}\" would traverse top directory", path));
      } else {
        parts.pop_back();
      }
    } else {
      parts.push_back(component);
    }
  }

  std::string result;
  if (is_absolute(path)) {
    result = "/";
  }

  result += core::string::join(parts.begin(), parts.end(), "/");
  return result;
}

std::string join(const std::string &front, const std::string &back) {
  if (is_absolute(back)) {
    const std::string::size_type pos = back.find_first_not_of("/");
    if (pos == std::string::npos) {
      return "/";
    } else {
      return back.substr(pos - 1);
    }
  } else {
    const std::string::size_type pos = front.find_last_not_of("/");
    if (pos == std::string::npos) {
      if (front.empty()) {
        return back;
      } else {
        return "/" + back;
      }
    } else {
      if (back.empty()) {
        return front.substr(0, pos + 1);
      } else {
        return front.substr(0, pos + 1) + "/" + back;
      }
    }
  }
}

bool is_relative_to(const std::string &path, const std::string &root) {
  assert(!root.empty());

  std::string::size_type n = root.find_last_not_of("/");
  if (n == std::string::npos) {
    n = 0;
  } else {
    ++n;
  }

  return path.compare(0, n, root, 0, n) == 0 &&
         ((n > 0 && path.length() == n) ||
          (path.length() > n && path[n] == '/'));
}

std::string make_relative(const std::string &path, const std::string &root) {
  assert(!root.empty());

  // TODO(armin): share code to find n with is_relative
  std::string::size_type n = root.find_last_not_of("/");
  if (n == std::string::npos) {
    n = 0;
  } else {
    ++n;
  }

  if (path.empty() || path.compare(0, n, root, 0, n) != 0) {
    throw std::invalid_argument(
        fmt::format("\"{}\" is not relative to \"{}\"", path, root));
  }

  const std::string::size_type start = path.find_first_not_of("/", n);
  if (start == n) {
    throw std::invalid_argument(
        fmt::format("\"{}\" is not relative to \"{}\"", path, root));
  }

  if (start == std::string::npos) {
    return ".";
  }

  return path.substr(start);
}

} // namespace freeisle::fs::path
