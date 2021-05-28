#include "time/SystemClock.hh"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <exception>
#include <system_error>

namespace freeisle::time {

namespace {

int64_t query_clock(clockid_t clk) {
  struct timespec ts;
  const int ret = clock_gettime(clk, &ts);

  if (ret < 0) {
    // This should not happen, both CLOCK_REALTIME and CLOCK_MONOTONIC
    // should be supported on a reasonably modern Linux system.
    throw std::system_error(
        std::make_error_code(std::errc::operation_not_supported));
  }

  return static_cast<int64_t>(ts.tv_sec) * 1000000 +
         static_cast<int64_t>(ts.tv_nsec / 1000);
}

} // namespace

SystemClock::SystemClock()
    : monotonic_time_offset(query_clock(CLOCK_MONOTONIC)) {}

Instant SystemClock::get_time() {
  const int64_t clockval = query_clock(CLOCK_REALTIME);
  return Instant::unix_usec(clockval);
}

Duration SystemClock::get_monotonic_time() {
  const int64_t clockval = query_clock(CLOCK_MONOTONIC);
  return Duration::usec(clockval - monotonic_time_offset);
}

} // namespace freeisle::time
