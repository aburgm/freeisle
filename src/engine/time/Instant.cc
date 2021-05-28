#include "time/Instant.hh"

#include <ctime>

namespace freeisle::time {

Instant Instant::unix_sec(int64_t timestamp) {
  return Instant(Duration::sec(timestamp));
}

Instant Instant::unix_msec(int64_t timestamp_ms) {
  return Instant(Duration::msec(timestamp_ms));
}

Instant Instant::unix_usec(int64_t timestamp_us) {
  return Instant(Duration::usec(timestamp_us));
}

Instant Instant::gregorian(const Gregorian &gregorian) {
  // There is no strict reason for this restriction, but it
  // seems reasonable, and should protect us from timegm
  // ever returning -1 and the result overflowing
  // or underflowing.
  assert(gregorian.year >= 0);
  assert(gregorian.year <= 9999);
  assert(gregorian.month >= 1);
  assert(gregorian.month <= 12);

  struct tm tm {};
  tm.tm_year = gregorian.year - 1900;
  tm.tm_mon = gregorian.month - 1;
  tm.tm_mday = gregorian.day;
  tm.tm_hour = gregorian.hour;
  tm.tm_min = gregorian.minute;
  tm.tm_sec = gregorian.second;

  const time_t result = timegm(&tm);
  assert(result != static_cast<time_t>(-1));

  int64_t result64 = static_cast<int64_t>(result);
  assert(result64 < std::numeric_limits<int64_t>::max() / 1000000);
  assert(result64 > std::numeric_limits<int64_t>::min() / 1000000);

  int64_t result64_usec = result64 * 1000000;
  assert(result64 < 0 || std::numeric_limits<int64_t>::max() - result64 >=
                             static_cast<int64_t>(gregorian.microsecond));

  return Instant(Duration::usec(result64_usec +
                                static_cast<int64_t>(gregorian.microsecond)));
}

Instant Instant::gregorian(int32_t year, uint32_t month, uint32_t day,
                           uint32_t hour, uint32_t minute, uint32_t second,
                           uint32_t microsecond) {
  return gregorian(Gregorian{
      .year = year,
      .month = month,
      .day = day,
      .hour = hour,
      .minute = minute,
      .second = second,
      .microsecond = microsecond,
  });
}

int64_t Instant::unix_sec() const { return val.sec<int64_t>(); }

int64_t Instant::unix_msec() const { return val.msec<int64_t>(); }

int64_t Instant::unix_usec() const { return val.usec<int64_t>(); }

Instant::Gregorian Instant::break_down() const {
  struct tm tm;

  int64_t val_usec = val.usec<int64_t>();
  time_t val_t = val_usec / 1000000;

  struct tm *res = gmtime_r(&val_t, &tm);
  assert(res != nullptr);

  return Gregorian{
      .year = static_cast<int32_t>(tm.tm_year) + 1900,
      .month = static_cast<uint32_t>(tm.tm_mon + 1),
      .day = static_cast<uint32_t>(tm.tm_mday),
      .hour = static_cast<uint32_t>(tm.tm_hour),
      .minute = static_cast<uint32_t>(tm.tm_min),
      .second = static_cast<uint32_t>(tm.tm_sec),
      .microsecond = static_cast<uint32_t>(val_usec % 1000000),
  };
}

Duration Instant::operator-(Instant rhs) const { return val - rhs.val; }

Instant &Instant::operator+=(Duration rhs) {
  val += rhs;
  return *this;
}

Instant &Instant::operator-=(Duration rhs) {
  val -= rhs;
  return *this;
}

} // namespace freeisle::time
