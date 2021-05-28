#pragma once

#include "core/Color.hh"
#include "core/Grid.hh"
#include "log/Logger.hh"

#include <cstdint>
#include <vector>

namespace freeisle::png {

/**
 * Decode a PNG-encoded image into an 8-bit RGB image.
 */
core::Grid<core::color::Rgb8> decode_rgb8(const uint8_t *data, uint64_t len,
                                          log::Logger logger);

/**
 * Encode an 8-bit RGB image as a PNG bytestream.
 */
std::vector<uint8_t> encode_rgb8(const core::Grid<core::color::Rgb8> &image,
                                 log::Logger logger);

} // namespace freeisle::png
