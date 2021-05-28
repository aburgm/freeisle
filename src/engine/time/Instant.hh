#pragma once

#include "time/Duration.hh"

#include <cstdint>

namespace freeisle::time {

/**
 * An instant in time, represented by a duration since the Unix epoch,
 * with microsecond precision. Does not handle leap seconds.
 */
class Instant {
public:
  /**
   * An instant broken down to the Gregorian calendar. The time portion
   * represents UTC time.
   */
  struct Gregorian {
    /**
     * Calendar year.
     */
    int32_t year;

    /**
     * Month of the year, a numerical value between 1 and 12.
     */
    uint32_t month;

    /**
     * Day of the month, a numerical value between 1 and 31.
     */
    uint32_t day;

    /**
     * Hour within a day, a numerical value between 0 and 23.
     */
    uint32_t hour;

    /**
     * Minute within the hour, a numerical value between 0 and 59.
     */
    uint32_t minute;

    /**
     * Second within the minute, a numerical value between 0 and 60.
     */
    uint32_t second;

    /**
     * Microsecond within the second, a numerical value between 0 and
     * 999999.
     */
    uint32_t microsecond;
  };

  /**
   * A default-constructed instant refers to the origin of the UNIX epoch,
   * 1970-01-01 00:00:00.000000.
   */
  Instant() = default;

  /**
   * Create an instant from a unix timestamp in seconds.
   */
  static Instant unix_sec(int64_t timestamp);

  /**
   * Create an instant from a unix timestamp in milliseconds.
   */
  static Instant unix_msec(int64_t timestamp_ms);

  /**
   * Create an instant from a unix timestamp in microseconds.
   */
  static Instant unix_usec(int64_t timestamp_us);

  /**
   * Construct an instant from a Gregorian representation.
   * The year must be between 0 and 9999, and all fields
   * of the Gregorian structure must be populated correctly.
   */
  static Instant gregorian(const Gregorian &gregorian);

  /**
   * Construct an instant from a Gregorian representation,
   * with the individual components as separate arguments.
   */
  static Instant gregorian(int32_t year, uint32_t month, uint32_t day,
                           uint32_t hour = 0, uint32_t minute = 0,
                           uint32_t second = 0, uint32_t microsecond = 0);

  /**
   * Returns the UNIX timestamp represented by this instant, in seconds.
   * Fractional seconds are rounded toward the nearest second.
   */
  int64_t unix_sec() const;

  /**
   * Returns the UNIX timestamp represented by this instant, in milliseconds.
   * Fractional milliseconds are rounded toward the nearest millisecond.
   */
  int64_t unix_msec() const;

  /**
   * Returns the UNIX timestamp represented by this instant, in microseconds.
   */
  int64_t unix_usec() const;

  /**
   * Break down the instant into the components of the
   * Gregorian calendar.
   */
  Gregorian break_down() const;

  Duration operator-(Instant rhs) const;
  Instant &operator+=(Duration rhs);
  Instant &operator-=(Duration rhs);

  bool operator==(Instant rhs) const { return val == rhs.val; }

  bool operator!=(Instant rhs) const { return val == rhs.val; }

  bool operator<(Instant rhs) const { return val < rhs.val; }

  bool operator>(Instant rhs) const { return val > rhs.val; }

  bool operator<=(Instant rhs) const { return val <= rhs.val; }

  bool operator>=(Instant rhs) const { return val >= rhs.val; }

private:
  explicit Instant(Duration d) : val(d) {}

  Duration val;
};

inline Instant operator+(Instant lhs, Duration rhs) { return lhs += rhs; }

inline Instant operator-(Instant lhs, Duration rhs) { return lhs -= rhs; }

} // namespace freeisle::time
