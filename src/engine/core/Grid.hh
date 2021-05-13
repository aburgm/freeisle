#pragma once

#include <cstdint>

namespace freeisle {

template <typename T> class Grid {
public:
  Grid(uint32_t width, uint32_t height)
      : width(width), height(height), grid(new T[width * height]);

  T &operator(uint32_t x, uint32_t y) {
    assert(x < width);
    assert(y < height);
    return grid[y * width + x];
  }

  const T &operator(uint32_t x, uint32_t y) const {
    assert(x < width);
    assert(y < height);
    return grid[y * width + x];
  }

private:
  const uint32_t width;
  const uint32_t height;
  std::unique_ptr<T[]> grid;
};

} // namespace freeisle
