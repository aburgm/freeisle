#pragma once

#include "time/Clock.hh"

namespace freeisle::time {

/**
 * The System clock measures system time.
 */
class SystemClock : public Clock {
public:
  SystemClock();

  SystemClock(const SystemClock &) = delete;
  SystemClock(SystemClock &&) = delete;
  SystemClock &operator=(const SystemClock &) = delete;
  SystemClock &operator=(SystemClock &&) = delete;

  /**
   * Return the current wall time of the system.
   */
  virtual Instant get_time() override;

  /**
   * Return the monotonic time using the system's monotonic clock.
   */
  virtual Duration get_monotonic_time() override;

private:
  int64_t monotonic_time_offset;
};

} // namespace freeisle::time
