#include "time/StopWatch.hh"

#include <gtest/gtest.h>

namespace {

class MockClock : public freeisle::time::Clock {
public:
  virtual freeisle::time::Instant get_time() override {
    return freeisle::time::Instant::unixSec(0) + d;
  }

  virtual freeisle::time::Duration get_monotonic_time() override { return d; }

  void advance_time(freeisle::time::Duration duration) { d += duration; }

private:
  freeisle::time::Duration d;
};

} // namespace

TEST(StopWatch, Default) {
  MockClock clock;
  freeisle::time::StopWatch watch(&clock);
  EXPECT_EQ(watch.elapsed(), freeisle::time::Duration::usec(0));

  clock.advance_time(freeisle::time::Duration::msec(100));
  EXPECT_EQ(watch.elapsed(), freeisle::time::Duration::msec(100));
}
