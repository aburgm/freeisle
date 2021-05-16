#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace freeisle::time {

/**
 * Duration represents the duration of time between two time instants
 * (or points). It represents the duration as the number of microseconds
 * between the two instants.
 *
 * A duration can be negative. Valid representions range between
 * -UINT64_MAX..UINT64_MAX. When doing arithmetics with durations,
 * values and the result would overflow, it is instead clamped to lie
 * within the valid range.
 */
class Duration {
public:
  /**
   * The default constructor creates a duration of 0 microseconds.
   */
  Duration() : us(0) {}

  /**
   * Construct a duration from a specified number of seconds.
   */
  template <typename T> static Duration sec(T duration_s) {
    return construct<T, 1000000>(duration_s);
  }

  /**
   * Construct a duration from a specified number of milliseconds.
   */
  template <typename T> static Duration msec(T duration_ms) {
    return construct<T, 1000>(duration_ms);
  }

  /**
   * Construct a duration from a specified number of microseconds.
   */
  template <typename T> static Duration usec(T duration_us) {
    return construct<T, 1>(duration_us);
  }

  /**
   * Return the number of seconds represented by this duration.
   */
  template <typename T> T sec() const { return deconstruct<T, 1000000>(); }

  /**
   * Return the number of milliseconds represented by this duration.
   */
  template <typename T> T msec() const { return deconstruct<T, 1000>(); }

  /**
   * Return the number of microseconds represented by this duration.
   */
  template <typename T> T usec() const { return deconstruct<T, 1>(); }

  Duration operator-() const { return Duration(-us); }

  Duration &operator+=(Duration other) {
    if (other.us < 0) {
      return *this -= (-other);
    }

    if (us > max - other.us) {
      us = max;
    } else {
      us += other.us;
    }

    return *this;
  }

  Duration &operator-=(Duration other) {
    if (other.us < 0) {
      return *this += (-other);
    }

    if (us < min + other.us) {
      us = min;
    } else {
      us -= other.us;
    }

    return *this;
  }

  template <typename T> Duration &operator*=(const T &rhs) {
    if (rhs == T(0)) {
      us = 0;
      return *this;
    }

    if (std::abs(rhs) < T(1)) {
      // T must be floating point
      return *this /= (T(1) / rhs);
    }

    if (rhs > T(0)) {
      if (us > static_cast<int64_t>(max / rhs)) {
        us = max;
      } else if (us < static_cast<int64_t>(min / rhs)) {
        us = min;
      } else {
        us *= rhs;
      }
    } else {
      if (us > static_cast<int64_t>(max / -rhs)) {
        us = min;
      } else if (us < static_cast<int64_t>(min / -rhs)) {
        us = max;
      } else {
        us *= rhs;
      }
    }

    return *this;
  }

  template <typename T> Duration &operator/=(const T &rhs) {
    assert(rhs != T(0));

    if (std::abs(rhs) < T(1)) {
      // T must be floating point
      *this *= (T(1) / rhs);
      return *this;
    }

    us /= rhs;
    return *this;
  }

  bool operator==(Duration rhs) const { return us == rhs.us; }

  bool operator<(Duration rhs) const { return us < rhs.us; }

  bool operator>(Duration rhs) const { return us > rhs.us; }

  bool operator<=(Duration rhs) const { return us <= rhs.us; }

  bool operator>=(Duration rhs) const { return us >= rhs.us; }

private:
  Duration(int64_t us) : us(us) {}

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_floating_point<T>::value,
                                    int64_t>::type = 0>
  static Duration construct(T duration) {
    constexpr T max_as_t = static_cast<T>(max / Frac);
    constexpr T min_as_t = static_cast<T>(min / Frac);

    if (duration > max_as_t) {
      return Duration(max);
    } else if (duration < min_as_t) {
      return Duration(min);
    }

    return Duration(static_cast<int64_t>(std::round(duration * Frac)));
  }

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_integral<T>::value &&
                                        std::is_unsigned<T>::value,
                                    int64_t>::type = 0>
  static Duration construct(T duration) {
    if (sizeof(T) >= sizeof(int64_t)) {
      constexpr T max_as_t = static_cast<T>(max / Frac);
      if (duration > max_as_t) {
        return Duration(max);
      }
      return Duration(static_cast<int64_t>(duration) * Frac);
    } else {
      if (static_cast<int64_t>(duration) > max / Frac) {
        return Duration(max);
      }
      return Duration(static_cast<int64_t>(duration) * Frac);
    }
  }

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_integral<T>::value &&
                                        std::is_signed<T>::value,
                                    int64_t>::type = 0>
  static Duration construct(T duration) {
    if (sizeof(T) > sizeof(int64_t)) {
      constexpr T max_as_t = static_cast<T>(max / Frac);
      constexpr T min_as_t = static_cast<T>(min / Frac);

      if (duration > max_as_t) {
        return Duration(max);
      } else if (duration < min_as_t) {
        return Duration(max);
      }

      return Duration(static_cast<int64_t>(duration) * Frac);
    } else {
      if (static_cast<int64_t>(duration) > max / Frac) {
        return Duration(max);
      } else if (static_cast<int64_t>(duration) < min / Frac) {
        return Duration(min);
      }

      return Duration(static_cast<int64_t>(duration) * Frac);
    }
  }

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_floating_point<T>::value,
                                    int64_t>::type = 0>
  T deconstruct() const {
    return T(us / Frac) + T(us % Frac) / T(Frac);
  }

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_integral<T>::value &&
                                        std::is_unsigned<T>::value,
                                    int64_t>::type = 0>
  T deconstruct() const {
    if (us < 0) {
      return T(0);
    }

    constexpr T max_t = std::numeric_limits<T>::max();
    if (sizeof(T) < sizeof(us)) {
      // might not fit
      if (us / Frac > static_cast<int64_t>(max_t)) {
        return max_t;
      }
    }

    T t(us / Frac);

    if (t < max_t && us % Frac > (Frac - 1) / 2) {
      ++t;
    }

    return t;
  }

  template <typename T, int64_t Frac,
            typename std::enable_if<std::is_integral<T>::value &&
                                        std::is_signed<T>::value,
                                    int64_t>::type = 0>
  T deconstruct() const {
    constexpr T max_t = std::numeric_limits<T>::max();
    constexpr T min_t = -max_t;

    if (sizeof(T) < sizeof(us)) {
      if (us / Frac > static_cast<int64_t>(max_t)) {
        return max_t;
      } else if (us / Frac < static_cast<int64_t>(min_t)) {
        return min_t;
      }
    }

    T t(us / Frac);

    if (t < max_t && us > 0 && us % Frac > (Frac - 1) / 2) {
      ++t;
    }

    if (t > min_t && us < 0 && us % Frac < -(Frac - 1) / 2) {
      --t;
    }

    return t;
  }

  int64_t us;

  /**
   * Maximum number of microseconds we can represent.
   */
  static constexpr int64_t max = std::numeric_limits<int64_t>::max();

  /**
   * Minimum number of microseconds we can represent. Note that this is
   * defined differently from std::numeric_limits<int64_t>::min(), mostly
   * because it makes all the bounds checks symmetric.
   * It also reserves one value to represent an invalid duration, although
   * it is not used as such yet.
   */
  static constexpr int64_t min = -std::numeric_limits<int64_t>::max();
};

inline Duration operator+(Duration lhs, Duration rhs) { return lhs += rhs; }

inline Duration operator-(Duration lhs, Duration rhs) { return lhs -= rhs; }

template <typename T> inline Duration operator*(Duration lhs, T rhs) {
  return lhs *= rhs;
}

template <typename T> inline Duration operator*(T lhs, Duration rhs) {
  return rhs *= lhs;
}

template <typename T> inline Duration operator/(Duration lhs, T rhs) {
  return lhs /= rhs;
}

} // namespace freeisle::time
