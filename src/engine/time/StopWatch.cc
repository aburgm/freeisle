#include "time/StopWatch.hh"

namespace freeisle::time {

StopWatch::StopWatch(Clock *clock)
    : clock(clock), startedAt(clock->get_monotonic_time()) {}

Duration StopWatch::elapsed() const {
  return clock->get_monotonic_time() - startedAt;
}

} // namespace freeisle::time
