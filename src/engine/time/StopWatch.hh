#pragma once

#include "time/Clock.hh"
#include "time/Duration.hh"

namespace freeisle::time {

/**
 * StopWatch is used to measure the time passed two instants
 * of time.
 */
class StopWatch {
public:
  /**
   * Create a new stop watch using the given clock. The watch is
   * "started" immediately upon creation.
   */
  StopWatch(Clock *clock);

  StopWatch(const StopWatch &) = delete;
  StopWatch(StopWatch &&) = delete;
  StopWatch &operator=(const StopWatch &) = delete;
  StopWatch &operator=(StopWatch &&) = delete;

  /**
   * Returns the time that has elapsed since the StopWatch was created.
   */
  Duration elapsed() const;

private:
  Clock *const clock;
  const Duration startedAt;
};

} // namespace freeisle::time
