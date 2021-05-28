#include "time/SystemClock.hh"

#include <gtest/gtest.h>

#include <gcov.h>

extern "C" {

// We are mocking the system clock by LD_PRELOADing an alternative
// clock_gettime(). With this function we can specify whether we
// want that clock_gettime() call to fail or not.
void set_clock_fail(int) __attribute__((weak));
}

TEST(SystemClock, Default) {
  set_clock_fail(0);

  freeisle::time::SystemClock clock;
  const freeisle::time::Instant inst = clock.get_time();
  EXPECT_EQ(inst, freeisle::time::Instant::unix_sec(1621283246));

  const freeisle::time::Duration monotonic1 = clock.get_monotonic_time();
  const freeisle::time::Duration monotonic2 = clock.get_monotonic_time();
  EXPECT_EQ(monotonic2 - monotonic1, freeisle::time::Duration::sec(1));
}

TEST(SystemClock, ClockFail) {
  set_clock_fail(1);
  EXPECT_THROW(freeisle::time::SystemClock(), std::exception);
}
