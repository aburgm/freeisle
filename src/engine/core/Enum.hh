#pragma once

#include <cstdint>
#include <cstring>

namespace freeisle::core {

/**
 * Given an enum type T, this specifies a mapping between a value
 * of the enum and a string representation of it.
 */
template <typename T> struct EnumEntry {
  T value;
  const char *str;
};

/**
 * Given a value from an enum type, convert it to a string.
 */
template <typename T, uint32_t N>
const char *to_string(const EnumEntry<T> (&entries)[N], T value) {
  for (EnumEntry<T> entry : entries) {
    if (entry.value == value) {
      return entry.str;
    }
  }

  return nullptr;
}

/**
 * Given a string value, convert to an enum type.
 */
template <typename T, uint32_t N>
const T *from_string(const EnumEntry<T> (&entries)[N], const char *str) {
  for (const EnumEntry<T> &entry : entries) {
    if (std::strcmp(entry.str, str) == 0) {
      return &entry.value;
    }
  }

  return nullptr;
}

} // namespace freeisle::core
