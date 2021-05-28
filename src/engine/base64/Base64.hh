#pragma once

#include <cstdint>

namespace freeisle::base64 {

template <uint64_t X> constexpr uint64_t round_to_next_multiple_of(uint64_t v) {
  static_assert((X & (X - 1)) == 0, "X must be a power of 2");
  return (v + (X - 1)) & ~(X - 1);
}

/**
 * Encodes a given byte sequence in base-64.
 * Result must be a at least `round_to_next_multiple_of<4>(len * 4 / 3)` bytes
 * long.
 */
uint64_t encode(const uint8_t *data, uint64_t len, uint8_t *result);

/**
 * Decodes a byte sequence from base64-encoded data.
 * Result must be at least `(len * 3 + 3) / 4` bytes long.
 *
 * Throws std::invalid_argument if the input is incorrecetly formatted.
 */
uint64_t decode(const uint8_t *data, uint64_t len, uint8_t *result);

} // namespace freeisle::base64
