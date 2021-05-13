#pragma once

namespace freeisle {

template <typename T> class Bitmask {
public:
  Bitmask() = default;
  Bitmask(T v) : val(1 << static_cast<uint32_t>(v)) {}

  Bitmask<T> operator|(Bitmask<T> other) const {
    return Bitmask<T>(val | other.val);
  }

  Bitmask<T> operator&(Bitmask<T> other) const {
    return Bitmask<T>(val & other.val);
  }

  bool is_set(T bit) const { return (*this & bit).val != 0; }
  void toggle(T bit) { val = val ^ bit; }

private:
  explicit Bitmask<T>(uint32_t val) : val(val) {}

  uint32_t val;
};

} // namespace freeisle
