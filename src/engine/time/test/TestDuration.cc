#include "time/Duration.hh"

#include <gtest/gtest.h>

#include <limits>

TEST(Duration, Default) {
  freeisle::time::Duration duration;
  EXPECT_EQ(duration.sec<int32_t>(), 0);
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), 0);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), 0.f);
  EXPECT_EQ(duration.sec<double>(), 0.);

  EXPECT_EQ(duration.msec<int32_t>(), 0);
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), 0);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), 0.f);
  EXPECT_EQ(duration.msec<double>(), 0.);

  EXPECT_EQ(duration.usec<int32_t>(), 0);
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), 0);
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), 0.f);
  EXPECT_EQ(duration.usec<double>(), 0.);
}

TEST(Duration, ConstructFromSecInt) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(2);

  EXPECT_EQ(duration.sec<int32_t>(), 2);
  EXPECT_EQ(duration.sec<uint32_t>(), 2);
  EXPECT_EQ(duration.sec<int64_t>(), 2);
  EXPECT_EQ(duration.sec<uint64_t>(), 2);
  EXPECT_EQ(duration.sec<float>(), 2.f);
  EXPECT_EQ(duration.sec<double>(), 2.);

  EXPECT_EQ(duration.msec<int32_t>(), 2000);
  EXPECT_EQ(duration.msec<uint32_t>(), 2000);
  EXPECT_EQ(duration.msec<int64_t>(), 2000);
  EXPECT_EQ(duration.msec<uint64_t>(), 2000);
  EXPECT_EQ(duration.msec<float>(), 2000.f);
  EXPECT_EQ(duration.msec<double>(), 2000.);

  EXPECT_EQ(duration.usec<int32_t>(), 2000000);
  EXPECT_EQ(duration.usec<uint32_t>(), 2000000);
  EXPECT_EQ(duration.usec<int64_t>(), 2000000);
  EXPECT_EQ(duration.usec<uint64_t>(), 2000000);
  EXPECT_EQ(duration.usec<float>(), 2000000.f);
  EXPECT_EQ(duration.usec<double>(), 2000000.);
}

TEST(Duration, ConstructFromSecFloatRoundDown) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(2.4f);

  EXPECT_EQ(duration.sec<int32_t>(), 2);
  EXPECT_EQ(duration.sec<uint32_t>(), 2);
  EXPECT_EQ(duration.sec<int64_t>(), 2);
  EXPECT_EQ(duration.sec<uint64_t>(), 2);
  EXPECT_EQ(duration.sec<float>(), 2.4f);
  EXPECT_EQ(duration.sec<double>(), 2.4);

  EXPECT_EQ(duration.msec<int32_t>(), 2400);
  EXPECT_EQ(duration.msec<uint32_t>(), 2400);
  EXPECT_EQ(duration.msec<int64_t>(), 2400);
  EXPECT_EQ(duration.msec<uint64_t>(), 2400);
  EXPECT_EQ(duration.msec<float>(), 2400.f);
  EXPECT_EQ(duration.msec<double>(), 2400.);

  EXPECT_EQ(duration.usec<int32_t>(), 2400000);
  EXPECT_EQ(duration.usec<uint32_t>(), 2400000);
  EXPECT_EQ(duration.usec<int64_t>(), 2400000);
  EXPECT_EQ(duration.usec<uint64_t>(), 2400000);
  EXPECT_EQ(duration.usec<float>(), 2400000.f);
  EXPECT_EQ(duration.usec<double>(), 2400000.);
}

TEST(Duration, ConstructFromSecFloatRoundUp) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(2.6f);

  EXPECT_EQ(duration.sec<int32_t>(), 3);
  EXPECT_EQ(duration.sec<uint32_t>(), 3);
  EXPECT_EQ(duration.sec<int64_t>(), 3);
  EXPECT_EQ(duration.sec<uint64_t>(), 3);
  EXPECT_EQ(duration.sec<float>(), 2.6f);
  EXPECT_EQ(duration.sec<double>(), 2.6);

  EXPECT_EQ(duration.msec<int32_t>(), 2600);
  EXPECT_EQ(duration.msec<uint32_t>(), 2600);
  EXPECT_EQ(duration.msec<int64_t>(), 2600);
  EXPECT_EQ(duration.msec<uint64_t>(), 2600);
  EXPECT_EQ(duration.msec<float>(), 2600.f);
  EXPECT_EQ(duration.msec<double>(), 2600.);

  EXPECT_EQ(duration.usec<int32_t>(), 2600000);
  EXPECT_EQ(duration.usec<uint32_t>(), 2600000);
  EXPECT_EQ(duration.usec<int64_t>(), 2600000);
  EXPECT_EQ(duration.usec<uint64_t>(), 2600000);
  EXPECT_EQ(duration.usec<float>(), 2600000.f);
  EXPECT_EQ(duration.usec<double>(), 2600000.);
}

TEST(Duration, ConstructFromSecIntNegative) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(-2);

  EXPECT_EQ(duration.sec<int32_t>(), -2);
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), -2);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), -2.f);
  EXPECT_EQ(duration.sec<double>(), -2.);

  EXPECT_EQ(duration.msec<int32_t>(), -2000);
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), -2000);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), -2000.f);
  EXPECT_EQ(duration.msec<double>(), -2000.);

  EXPECT_EQ(duration.usec<int32_t>(), -2000000);
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), -2000000);
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), -2000000.f);
  EXPECT_EQ(duration.usec<double>(), -2000000.);
}

TEST(Duration, ConstructFromSecFloatRoundDownNegative) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(-2.4f);

  EXPECT_EQ(duration.sec<int32_t>(), -2);
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), -2);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), -2.4f);
  EXPECT_EQ(duration.sec<double>(), -2.4);

  EXPECT_EQ(duration.msec<int32_t>(), -2400);
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), -2400);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), -2400.f);
  EXPECT_EQ(duration.msec<double>(), -2400.);

  EXPECT_EQ(duration.usec<int32_t>(), -2400000);
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), -2400000);
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), -2400000.f);
  EXPECT_EQ(duration.usec<double>(), -2400000.);
}

TEST(Duration, ConstructFromSecFloatRoundUpNegative) {
  freeisle::time::Duration duration = freeisle::time::Duration::sec(-2.6f);

  EXPECT_EQ(duration.sec<int32_t>(), -3);
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), -3);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), -2.6f);
  EXPECT_EQ(duration.sec<double>(), -2.6);

  EXPECT_EQ(duration.msec<int32_t>(), -2600);
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), -2600);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), -2600.f);
  EXPECT_EQ(duration.msec<double>(), -2600.);

  EXPECT_EQ(duration.usec<int32_t>(), -2600000);
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), -2600000);
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), -2600000.f);
  EXPECT_EQ(duration.usec<double>(), -2600000.);
}

TEST(Duration, ConstructBig) {
  const int64_t secs = 1000000000000;
  const int64_t msecs = secs * 1000;
  const int64_t usecs = msecs * 1000;

  freeisle::time::Duration duration = freeisle::time::Duration::msec(msecs);

  EXPECT_EQ(duration.sec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.sec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.sec<int64_t>(), secs);
  EXPECT_EQ(duration.sec<uint64_t>(), secs);
  EXPECT_EQ(duration.sec<float>(), secs);
  EXPECT_EQ(duration.sec<double>(), secs);

  EXPECT_EQ(duration.msec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.msec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.msec<int64_t>(), msecs);
  EXPECT_EQ(duration.msec<uint64_t>(), msecs);
  EXPECT_EQ(duration.msec<float>(), msecs);
  EXPECT_EQ(duration.msec<double>(), msecs);

  EXPECT_EQ(duration.usec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.usec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.usec<int64_t>(), usecs);
  EXPECT_EQ(duration.usec<uint64_t>(), usecs);
  EXPECT_EQ(duration.usec<float>(), usecs);
  EXPECT_EQ(duration.usec<double>(), usecs);
}

TEST(Duration, ConstructOverflow) {
  const int64_t secs = 1000000000000000;
  const int64_t msecs = secs * 1000;

  freeisle::time::Duration duration = freeisle::time::Duration::msec(msecs);

  EXPECT_EQ(duration.sec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.sec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.sec<int64_t>(), 9223372036855);
  EXPECT_EQ(duration.sec<uint64_t>(), 9223372036855);
  EXPECT_EQ(duration.sec<float>(), 9223372036854.776f);
  EXPECT_EQ(duration.sec<double>(), 9223372036854.776);

  EXPECT_EQ(duration.msec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.msec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.msec<int64_t>(), 9223372036854776);
  EXPECT_EQ(duration.msec<uint64_t>(), 9223372036854776);
  EXPECT_EQ(duration.msec<float>(), 9223372036854776.f);
  EXPECT_EQ(duration.msec<double>(), 9223372036854776.);

  EXPECT_EQ(duration.usec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.usec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.usec<int64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<uint64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<float>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<double>(), std::numeric_limits<int64_t>::max());
}

TEST(Duration, ConstructOverflowFp) {
  const int64_t secs = 1000000000000000;
  const int64_t msecs = secs * 1000;

  freeisle::time::Duration duration =
      freeisle::time::Duration::msec(static_cast<double>(msecs));

  EXPECT_EQ(duration.sec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.sec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.sec<int64_t>(), 9223372036855);
  EXPECT_EQ(duration.sec<uint64_t>(), 9223372036855);
  EXPECT_EQ(duration.sec<float>(), 9223372036854.776f);
  EXPECT_EQ(duration.sec<double>(), 9223372036854.776);

  EXPECT_EQ(duration.msec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.msec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.msec<int64_t>(), 9223372036854776);
  EXPECT_EQ(duration.msec<uint64_t>(), 9223372036854776);
  EXPECT_EQ(duration.msec<float>(), 9223372036854776.f);
  EXPECT_EQ(duration.msec<double>(), 9223372036854776.);

  EXPECT_EQ(duration.usec<int32_t>(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.usec<uint32_t>(), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(duration.usec<int64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<uint64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<float>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<double>(), std::numeric_limits<int64_t>::max());
}

TEST(Duration, ConstructUnderflow) {
  const int64_t secs = -1000000000000000;
  const int64_t msecs = secs * 1000;

  freeisle::time::Duration duration = freeisle::time::Duration::msec(msecs);

  EXPECT_EQ(duration.sec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), -9223372036855);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), -9223372036854.776f);
  EXPECT_EQ(duration.sec<double>(), -9223372036854.776);

  EXPECT_EQ(duration.msec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), -9223372036854776);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), -9223372036854776.f);
  EXPECT_EQ(duration.msec<double>(), -9223372036854776.);

  EXPECT_EQ(duration.usec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<double>(), -std::numeric_limits<int64_t>::max());
}

TEST(Duration, ConstructUnderflowFp) {
  const int64_t secs = -1000000000000000;
  const int64_t msecs = secs * 1000;

  freeisle::time::Duration duration =
      freeisle::time::Duration::msec(static_cast<double>(msecs));

  EXPECT_EQ(duration.sec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.sec<uint32_t>(), 0);
  EXPECT_EQ(duration.sec<int64_t>(), -9223372036855);
  EXPECT_EQ(duration.sec<uint64_t>(), 0);
  EXPECT_EQ(duration.sec<float>(), -9223372036854.776f);
  EXPECT_EQ(duration.sec<double>(), -9223372036854.776);

  EXPECT_EQ(duration.msec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.msec<uint32_t>(), 0);
  EXPECT_EQ(duration.msec<int64_t>(), -9223372036854776);
  EXPECT_EQ(duration.msec<uint64_t>(), 0);
  EXPECT_EQ(duration.msec<float>(), -9223372036854776.f);
  EXPECT_EQ(duration.msec<double>(), -9223372036854776.);

  EXPECT_EQ(duration.usec<int32_t>(), -std::numeric_limits<int32_t>::max());
  EXPECT_EQ(duration.usec<uint32_t>(), 0);
  EXPECT_EQ(duration.usec<int64_t>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<uint64_t>(), 0);
  EXPECT_EQ(duration.usec<float>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ(duration.usec<double>(), -std::numeric_limits<int64_t>::max());
}

TEST(Duration, Add) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(2);
  freeisle::time::Duration d2 = freeisle::time::Duration::sec(3);

  EXPECT_EQ((d1 + d2).sec<int32_t>(), 5);
}

TEST(Duration, AddNegative) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(2);
  freeisle::time::Duration d2 = freeisle::time::Duration::sec(-3);

  EXPECT_EQ((d1 + d2).sec<int32_t>(), -1);
}

TEST(Duration, AddOverflow) {
  freeisle::time::Duration d1 =
      freeisle::time::Duration::usec(std::numeric_limits<int64_t>::max() - 2);
  freeisle::time::Duration d2 = freeisle::time::Duration::usec(3);

  EXPECT_EQ((d1 + d2).usec<int64_t>(), std::numeric_limits<int64_t>::max());
}

TEST(Duration, Subtract) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(2);
  freeisle::time::Duration d2 = freeisle::time::Duration::sec(3);

  EXPECT_EQ((d1 - d2).sec<int32_t>(), -1);
}

TEST(Duration, SubtractNegative) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(2);
  freeisle::time::Duration d2 = freeisle::time::Duration::sec(-3);

  EXPECT_EQ((d1 - d2).sec<int32_t>(), 5);
}

TEST(Duration, SubtractOverflow) {
  freeisle::time::Duration d1 = freeisle::time::Duration::usec(-10);
  freeisle::time::Duration d2 =
      freeisle::time::Duration::usec(std::numeric_limits<int64_t>::max() - 2);

  EXPECT_EQ((d1 - d2).usec<int64_t>(), -std::numeric_limits<int64_t>::max());
}

TEST(Duration, Multiply) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(6);

  EXPECT_EQ((d1 * 5).sec<int32_t>(), 30);
  EXPECT_EQ((5 * d1).sec<int32_t>(), 30);

  EXPECT_EQ((d1 * -4).sec<int32_t>(), -24);
  EXPECT_EQ((-4 * d1).sec<int32_t>(), -24);
}

TEST(Duration, MultiplyFloat) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(6);

  EXPECT_EQ((d1 * 5.5).sec<int32_t>(), 33);
  EXPECT_EQ((5.5 * d1).sec<int32_t>(), 33);

  EXPECT_EQ((d1 * 0.5).sec<int32_t>(), 3);
  EXPECT_EQ((0.5 * d1).sec<int32_t>(), 3);

  EXPECT_EQ((d1 * 0.).sec<int32_t>(), 0);
  EXPECT_EQ((0. * d1).sec<int32_t>(), 0);

  EXPECT_EQ((d1 * -2.).sec<int32_t>(), -12);
  EXPECT_EQ((-2. * d1).sec<int32_t>(), -12);
}

TEST(Duration, MultiplyOverflowPositive) {
  freeisle::time::Duration d1 =
      freeisle::time::Duration::sec(std::numeric_limits<int64_t>::max() - 2);

  EXPECT_EQ((d1 * 2).usec<int64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ((2 * d1).usec<int64_t>(), std::numeric_limits<int64_t>::max());

  EXPECT_EQ((d1 * -2).usec<int64_t>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ((-2 * d1).usec<int64_t>(), -std::numeric_limits<int64_t>::max());
}

TEST(Duration, MultiplyOverflowNegative) {
  freeisle::time::Duration d1 =
      freeisle::time::Duration::sec(-std::numeric_limits<int64_t>::max() + 2);

  EXPECT_EQ((d1 * 2).usec<int64_t>(), -std::numeric_limits<int64_t>::max());
  EXPECT_EQ((2 * d1).usec<int64_t>(), -std::numeric_limits<int64_t>::max());

  EXPECT_EQ((d1 * -2).usec<int64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ((-2 * d1).usec<int64_t>(), std::numeric_limits<int64_t>::max());
}

TEST(Duration, Divide) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(6);

  EXPECT_EQ((d1 / 3).sec<int32_t>(), 2);
}

TEST(Duration, DivideFloat) {
  freeisle::time::Duration d1 = freeisle::time::Duration::sec(60);

  EXPECT_EQ((d1 / 2.5).sec<int32_t>(), 24);
  EXPECT_EQ((d1 / 0.5).sec<int32_t>(), 120);
}

TEST(Duration, DivideOverflow) {
  freeisle::time::Duration d1 =
      freeisle::time::Duration::sec(std::numeric_limits<int64_t>::max() - 2);

  EXPECT_EQ((d1 / 0.5).usec<int64_t>(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ((d1 / -0.5).usec<int64_t>(), -std::numeric_limits<int64_t>::max());
}
