#pragma once

#include <cassert>

namespace freeisle::core {

/**
 * A sentinel is a simple wrapper around a value of type T where a special
 * value of T represents an "invalid" state.
 */
template <typename T, T S> class Sentinel {
public:
  /**
   * Default constructor; constructs a sentinel in the invalid state.
   */
  Sentinel() : val(S) {}

  /**
   * Sentinel constructor. value must not represent the invalid state.
   */
  Sentinel(T val) : val(val) { assert(val != S); }

  /**
   * Returns true if the sentinel is not in the invalid state.
   */
  explicit operator bool() const { return val != S; }

  /**
   * Returns true if the sentinel is in the invalid state.
   */
  bool operator!() const { return val == S; }

  /**
   * Assign a value to the sentinel. value must not be the invalid state.
   * To set a sentinel to the invalid value, assign a default-constructed
   * sentinel object to it.
   */
  Sentinel &operator=(T v) {
    assert(v != S);
    val = v;
    return *this;
  }

  /**
   * Return the value behind the sentinel. Only valid if sentinel represents
   * a valid state.
   */
  const T &operator*() const {
    assert(val != S);
    return val;
  }

  /**
   * Return the value behind the sentinel. Only valid if sentinel represents
   * a valid state.
   */
  const T *operator->() {
    assert(val != S);
    return &val;
  }

private:
  T val;
};

} // namespace freeisle::core
