#pragma once

#include <cstdint>

namespace freeisle::core::color {

/**
 * Represents a color in the RGB (red green blue) colorspace.
 */
template <typename T> struct Rgb {
  /**
   * Red component of the color.
   */
  T r;

  /**
   * Green component of the color.
   */
  T g;

  /**
   * Blue component of the color.
   */
  T b;
};

/**
 * RGB Color with 8 bit color depth.
 */
using Rgb8 = Rgb<uint8_t>;

} // namespace freeisle::core::color
