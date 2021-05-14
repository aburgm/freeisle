#pragma once

#include <cassert>
#include <cstdint>
#include <memory>

namespace freeisle::core {

/**
 * Represents a 2-D grid of fixed width and height of some type T.
 */
template <typename T> class Grid {
public:
  /**
   * Create a new grid with the given dimensions. All elements are
   * default-initialized.
   */
  Grid(uint32_t width, uint32_t height)
      : width(width), height(height), grid(new T[width * height]()) {}

  /**
   * Access the element at the given location.
   */
  T &operator()(uint32_t x, uint32_t y) {
    assert(x < width);
    assert(y < height);
    return grid[y * width + x];
  }

  /**
   * Access the element at the given location.
   */
  const T &operator()(uint32_t x, uint32_t y) const {
    assert(x < width);
    assert(y < height);
    return grid[y * width + x];
  }

private:
  const uint32_t width;
  const uint32_t height;
  std::unique_ptr<T[]> grid;
};

} // namespace freeisle::core
