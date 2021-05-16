#pragma once

#include "time/Duration.hh"
#include "time/Instant.hh"

namespace freeisle::time {

/**
 * Clock is a simple interface to get the current time and perform
 * other time-based tasks.
 *
 * Clock being a generic interface allows mocking time in unit tests
 * or replay of previously recorded data.
 */
class Clock {
public:
  virtual ~Clock() = default;

  /**
   * Get the current (wall) time according to the clock.
   */
  virtual Instant get_time() = 0;

  /**
   * Get a monotonic time from the clock. Monotonic time is expressed
   * as a duration measured since the creation of the clock.
   */
  virtual Duration get_monotonic_time() = 0;
};

} // namespace freeisle::time
