#include "time/Instant.hh"

#include <gtest/gtest.h>

#include <limits>

TEST(Instant, Default) {
  freeisle::time::Instant instant;
  EXPECT_EQ(instant.unixSec(), 0);
  EXPECT_EQ(instant.unixMsec(), 0);
  EXPECT_EQ(instant.unixUsec(), 0);
}

TEST(Instant, ConstructFromSec) {
  freeisle::time::Instant instant = freeisle::time::Instant::unixSec(3);

  EXPECT_EQ(instant.unixSec(), 3);
  EXPECT_EQ(instant.unixMsec(), 3000);
  EXPECT_EQ(instant.unixUsec(), 3000000);
}

TEST(Instant, ConstructFromMsec) {
  freeisle::time::Instant instant = freeisle::time::Instant::unixMsec(3000);

  EXPECT_EQ(instant.unixSec(), 3);
  EXPECT_EQ(instant.unixMsec(), 3000);
  EXPECT_EQ(instant.unixUsec(), 3000000);
}

TEST(Instant, ConstructFromUsec) {
  freeisle::time::Instant instant = freeisle::time::Instant::unixUsec(3000000);

  EXPECT_EQ(instant.unixSec(), 3);
  EXPECT_EQ(instant.unixMsec(), 3000);
  EXPECT_EQ(instant.unixUsec(), 3000000);
}

TEST(Instant, ConstructGregorian) {
  const freeisle::time::Instant::Gregorian g{
      .year = 2021,
      .month = 5,
      .day = 16,
      .hour = 16,
      .minute = 1,
      .second = 31,
      .microsecond = 421991,
  };

  const freeisle::time::Instant instant = freeisle::time::Instant::gregorian(g);

  EXPECT_EQ(instant.unixSec(), 1621180891);
  EXPECT_EQ(instant.unixMsec(), 1621180891422);
  EXPECT_EQ(instant.unixUsec(), 1621180891421991);
}

TEST(Instant, ConstructGregorianRoundUp) {
  const freeisle::time::Instant::Gregorian g{
      .year = 2021,
      .month = 5,
      .day = 16,
      .hour = 16,
      .minute = 1,
      .second = 31,
      .microsecond = 521991,
  };

  const freeisle::time::Instant instant = freeisle::time::Instant::gregorian(g);

  EXPECT_EQ(instant.unixSec(), 1621180892);
  EXPECT_EQ(instant.unixMsec(), 1621180891522);
  EXPECT_EQ(instant.unixUsec(), 1621180891521991);
}

TEST(Instant, ConstructGregorianSeparate) {
  const freeisle::time::Instant instant =
      freeisle::time::Instant::gregorian(2021, 5, 16, 16, 1, 31, 421991);

  EXPECT_EQ(instant.unixSec(), 1621180891);
  EXPECT_EQ(instant.unixMsec(), 1621180891422);
  EXPECT_EQ(instant.unixUsec(), 1621180891421991);
}

TEST(Instant, BreakDown) {
  const freeisle::time::Instant instant =
      freeisle::time::Instant::unixUsec(1621180891421991);

  const freeisle::time::Instant::Gregorian g = instant.break_down();

  EXPECT_EQ(g.year, 2021);
  EXPECT_EQ(g.month, 5);
  EXPECT_EQ(g.day, 16);
  EXPECT_EQ(g.hour, 16);
  EXPECT_EQ(g.minute, 1);
  EXPECT_EQ(g.second, 31);
  EXPECT_EQ(g.microsecond, 421991);
}

TEST(Instant, SubtractTwoInstants) {
  const freeisle::time::Instant instant1 =
      freeisle::time::Instant::unixUsec(1621180881421991);
  const freeisle::time::Instant instant2 =
      freeisle::time::Instant::unixUsec(1621180891421991);
  const freeisle::time::Duration dur1 = instant2 - instant1;
  const freeisle::time::Duration dur2 = instant1 - instant2;

  EXPECT_EQ(dur1, -dur2);

  EXPECT_EQ(dur1.sec<int64_t>(), 10);
  EXPECT_EQ(dur2.sec<int64_t>(), -10);
}

TEST(Instant, InstantPlusDuration) {
  const freeisle::time::Instant instant =
      freeisle::time::Instant::unixUsec(1621180881421991);
  const freeisle::time::Duration dur = freeisle::time::Duration::sec(10);

  freeisle::time::Instant v1 = instant;
  const freeisle::time::Instant v2 = instant + dur;
  v1 += dur;

  EXPECT_EQ(v1.unixUsec(), 1621180891421991);
  EXPECT_EQ(v2.unixUsec(), 1621180891421991);

  freeisle::time::Instant v3 = instant;
  const freeisle::time::Instant v4 = instant + (-dur);
  v3 += (-dur);

  EXPECT_EQ(v3.unixUsec(), 1621180871421991);
  EXPECT_EQ(v4.unixUsec(), 1621180871421991);
}

TEST(Instant, InstantMinusDuration) {
  const freeisle::time::Instant instant =
      freeisle::time::Instant::unixUsec(1621180881421991);
  const freeisle::time::Duration dur = freeisle::time::Duration::sec(10);

  freeisle::time::Instant v1 = instant;
  const freeisle::time::Instant v2 = instant - dur;
  v1 -= dur;

  EXPECT_EQ(v1.unixUsec(), 1621180871421991);
  EXPECT_EQ(v2.unixUsec(), 1621180871421991);

  freeisle::time::Instant v3 = instant;
  const freeisle::time::Instant v4 = instant - (-dur);
  v3 -= (-dur);

  EXPECT_EQ(v3.unixUsec(), 1621180891421991);
  EXPECT_EQ(v4.unixUsec(), 1621180891421991);
}
