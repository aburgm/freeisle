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
      : width_(width), height_(height), grid_(new T[width * height]()) {}

  uint32_t width() const { return width_; }
  uint32_t height() const { return height_; }

  /**
   * Access the element at the given location.
   */
  T &operator()(uint32_t x, uint32_t y) {
    assert(x < width_);
    assert(y < height_);
    return grid_[y * width_ + x];
  }

  /**
   * Access the element at the given location.
   */
  const T &operator()(uint32_t x, uint32_t y) const {
    assert(x < width_);
    assert(y < height_);
    return grid_[y * width_ + x];
  }

private:
  const uint32_t width_;
  const uint32_t height_;
  std::unique_ptr<T[]> grid_;
};

} // namespace freeisle::core
