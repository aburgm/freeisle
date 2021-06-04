#pragma once

#include "core/Enum.hh"

namespace freeisle::core {

namespace detail {

template <typename T, uint32_t N, typename EnumType, typename... Tail>
class EnumMapImpl
    : public EnumMapImpl<T, N + static_cast<uint32_t>(EnumType::Num), Tail...> {
public:
  constexpr EnumMapImpl() = default;

  constexpr T &operator[](EnumType e) {
    return this->arr[N + static_cast<uint32_t>(e)];
  }
  constexpr const T &operator[](EnumType e) const {
    return this->arr[N + static_cast<uint32_t>(e)];
  }

  using EnumMapImpl<T, N + static_cast<uint32_t>(EnumType::Num),
                    Tail...>::operator[];
};

template <typename T, uint32_t N, typename EnumType>
class EnumMapImpl<T, N, EnumType> {
public:
  constexpr EnumMapImpl() = default;
  constexpr T *data() { return arr; }
  constexpr const T *data() const { return arr; }

  constexpr size_t size() const {
    return N + static_cast<uint32_t>(EnumType::Num);
  }

  constexpr T &operator[](EnumType e) {
    return arr[N + static_cast<uint32_t>(e)];
  }
  constexpr const T &operator[](EnumType e) const {
    return arr[N + static_cast<uint32_t>(e)];
  }

protected:
  T arr[N + static_cast<uint32_t>(EnumType::Num)];
};

} // namespace detail

/**
 * A mapping from each value of one or more enum types to a type T.
 * Each enum must have a member "Num" with the number of elements in the enum,
 * and the enum values must start at 0 and increment without gaps.
 */
template <typename T, typename... EnumTypes>
class EnumMap : public detail::EnumMapImpl<T, 0, EnumTypes...> {
public:
  constexpr EnumMap() {}
};

/**
 * Construct an EnumMap which maps every enum value to a string.
 */
template <typename EnumType, uint32_t N>
constexpr EnumMap<const char *, EnumType>
get_enum_names(const core::EnumEntry<EnumType> (&entries)[N]) {
  core::EnumMap<const char *, EnumType> names;
  for (core::EnumEntry<EnumType> entry : entries) {
    names[entry.value] = entry.str;
  }
  return names;
}

/**
 * Construct an EnumMap which maps every enum value to a string,
 * for an EnumMap spanning two enums.
 */
template <typename EnumType1, typename EnumType2, uint32_t N1, uint32_t N2>
constexpr EnumMap<const char *, EnumType1, EnumType2>
get_enum_names(const core::EnumEntry<EnumType1> (&entries1)[N1],
               const core::EnumEntry<EnumType2> (&entries2)[N2]) {
  // TODO(armin): how to make this generic for an arbitrary number of enum
  // types?
  core::EnumMap<const char *, EnumType1, EnumType2> names;
  for (core::EnumEntry<EnumType1> entry : entries1) {
    names[entry.value] = entry.str;
  }
  for (core::EnumEntry<EnumType2> entry : entries2) {
    names[entry.value] = entry.str;
  }
  return names;
}

} // namespace freeisle::core
