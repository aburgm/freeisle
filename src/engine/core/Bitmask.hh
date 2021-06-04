#pragma once

#include <cassert>
#include <cstdint>

namespace freeisle::core {

/**
 * A helper class to store a bitmask of values of some type T,
 * typically an enum. The enumerants must have numerical values
 * between 0 and 31.
 */
template <typename T> class Bitmask {
public:
  /**
   * Default constructor, constructs a bitmask with no bits set.
   */
  Bitmask() : val(0) {}

  /**
   * Construct a bitmask with all the given bits set.
   */
  template <typename... U> Bitmask(T v, U... tail) : Bitmask(tail...) {
    assert(static_cast<uint32_t>(v) < 8 * sizeof(uint32_t));
    val |= (1 << static_cast<uint32_t>(v));
  }

  /**
   * Computes the bitwise OR between two bitmasks.
   */
  Bitmask<T> &operator|=(Bitmask<T> other) {
    val |= other.val;
    return *this;
  }

  /**
   * Computes the bitwise OR between two bitmasks.
   */
  Bitmask<T> operator|(Bitmask<T> other) const {
    return Bitmask<T>(val | other.val);
  }

  /**
   * Computes the bitwise AND between two bitmasks.
   */
  Bitmask<T> operator&(Bitmask<T> other) const {
    return Bitmask<T>(val & other.val);
  }

  /**
   * Computes the bitwise XOR between two bitmasks.
   */
  Bitmask<T> operator^(Bitmask<T> other) const {
    return Bitmask<T>(val ^ other.val);
  }

  /**
   * Compares two bitmasks for equality.
   */
  bool operator==(Bitmask<T> other) const { return val == other.val; }

  /**
   * Compares two bitmasks for inequality.
   */
  bool operator!=(Bitmask<T> other) const { return val != other.val; }

  /**
   * Returns whether the bit corresponding to the given value
   * is set or not.
   */
  bool is_set(T bit) const { return are_all_set(bit); }

  /**
   * Toggles the best with the given value.
   */
  void toggle(T bit) { *this = *this ^ bit; }

  /**
   * Returns true if all bits in @param bits are set in the mask.
   */
  bool are_all_set(Bitmask<T> bits) const { return (*this & bits) == bits; }

  /**
   * Returns true if any of the bits in @param bits are set in the mask.
   */
  bool are_any_set(Bitmask<T> bits) const {
    return (*this & bits) != Bitmask<T>{};
  }

  /**
   * Returns true if there is at least one bit set in the mask.
   */
  bool are_any_set() const { return val != 0; }

  /**
   * Returns the number of bits set in the mask.
   */
  uint32_t n_set() const { return __builtin_popcount(val); }

private:
  explicit Bitmask(uint32_t val) : val(val) {}

  uint32_t val;
};

} // namespace freeisle::core
