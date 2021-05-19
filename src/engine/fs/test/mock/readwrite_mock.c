#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>

typedef ssize_t (*read_func)(int fd, void* buf, size_t count);
typedef ssize_t (*write_func)(int fd, const void* buf, size_t count);

static int next_read_error = 0;
static int next_write_error = 0;
static int next_write_length_is_zero = 0;

void set_next_read_error(int err) {
  next_read_error = err;
}

void set_next_write_error(int err) {
  next_write_error = err;
}

void set_next_write_length_to_zero() {
  next_write_length_is_zero = 1;
}
 
ssize_t read(int fd, void* buf, size_t count) {
  if (next_read_error != 0) {
    errno = next_read_error;
    next_read_error = 0;
    return -1;
  }

  read_func f = (read_func)dlsym(RTLD_NEXT, "read");
  assert(f != NULL);

  return f(fd, buf, count);
}

ssize_t write(int fd, const void* buf, size_t count) {
  if (next_write_error != 0) {
    errno = next_write_error;
    next_write_error = 0;
    return -1;
  }

  if (next_write_length_is_zero) {
    errno = 0;
    --next_write_length_is_zero;
    return 0;
  }

  write_func f = (write_func)dlsym(RTLD_NEXT, "write");
  assert(f != NULL);

  return f(fd, buf, count);
}
