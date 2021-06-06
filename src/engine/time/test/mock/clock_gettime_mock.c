#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <time.h>

static int clock_fail = 0;
static int call_count = 0;

void set_clock_fail(int fail) { clock_fail = fail; }

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
  if (clock_fail) {
    errno = EINVAL;
    return -1;
  }

  if (clk_id == CLOCK_REALTIME) {
    tp->tv_sec = 1621283246;
    tp->tv_nsec = 0;
  } else {
    tp->tv_sec = 500 + call_count;
    tp->tv_nsec = 0;
    ++call_count;
  }

  return 0;
}
