#include "fs/Directory.hh"

#include <fmt/format.h>

#include <stdexcept>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace freeisle::fs {

Directory::Directory() : fd(-1) {}

Directory::~Directory() {
  if (fd != -1) {
    ::close(fd);
  }
}

Directory::Directory(const char *path, Directory *dir) {
  int dirfd = AT_FDCWD;
  if (dir != nullptr) {
    dirfd = dir->fd;
  }

  fd = ::openat(dirfd, path, O_DIRECTORY | O_CLOEXEC);
  if (fd == -1) {
    throw std::runtime_error(fmt::format("Failed to open directory \"{}\": {}",
                                         path, ::strerror(errno)));
  }
}

Directory::Directory(Directory &&other) : fd(other.fd) { other.fd = -1; }

Directory &Directory::operator=(Directory &&other) {
  if (this == &other) {
    return *this;
  }

  if (fd != -1) {
    ::close(fd);
  }

  fd = other.fd;
  other.fd = -1;

  return *this;
}

Directory::operator bool() const { return fd != -1; }

} // namespace freeisle::fs
