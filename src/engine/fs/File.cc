#include "fs/File.hh"

#include <fmt/format.h>

#include <stdexcept>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace freeisle::fs {

File::File() : fd(-1) {}

File::File(const char *path, core::Bitmask<OpenMode> mode, Directory *dir)
    : fd(-1) {
  int flags = 0;

  assert(!mode.is_set(OpenMode::Truncate) || mode.is_set(OpenMode::Write));
  assert(!mode.is_set(OpenMode::FailIfExist) || mode.is_set(OpenMode::Create));

  if (mode.is_set(OpenMode::Read)) {
    if (mode.is_set(OpenMode::Write)) {
      flags |= O_RDWR;
    } else {
      flags |= O_RDONLY;
    }
  } else if (mode.is_set(OpenMode::Write)) {
    flags |= O_WRONLY;
  }

  if (mode.is_set(OpenMode::Create)) {
    flags |= O_CREAT;
  }

  if (mode.is_set(OpenMode::Truncate)) {
    flags |= O_TRUNC;
  }

  if (mode.is_set(OpenMode::FailIfExist)) {
    flags |= O_EXCL;
  }

  int dirfd = AT_FDCWD;
  if (dir != nullptr) {
    dirfd = dir->fd;
  }

  fd = ::openat(dirfd, path, flags, 0666);
  if (fd < 0) {
    throw std::runtime_error(
        fmt::format("Failed to open file \"{}\": {}", path, ::strerror(errno)));
  }
}

File::~File() {
  if (fd != -1) {
    ::close(fd);
  }
}

File::File(File &&other) : fd(other.fd) { other.fd = -1; }

File &File::operator=(File &&other) {
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

FileInfo File::info() const {
  struct stat buf;
  if (::fstat(fd, &buf) != 0) {
    throw std::runtime_error(
        fmt::format("Failed to stat: {}", ::strerror(errno)));
  }

  return FileInfo{
      .id =
          FileId{
              .dev = buf.st_dev,
              .ino = buf.st_ino,
          },
      .size = static_cast<uint64_t>(buf.st_size),
  };
}

uint64_t File::read(uint8_t *data, uint64_t length) {
  while (true) {
    const ssize_t res = ::read(fd, data, length);
    if (res < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(
          fmt::format("Failed to read: {}", ::strerror(errno)));
    }

    return static_cast<uint64_t>(res);
  }
}

uint64_t File::write(const uint8_t *data, uint64_t length) {
  while (true) {
    const ssize_t res = ::write(fd, data, length);
    if (res < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error(
          fmt::format("Failed to write: {}", ::strerror(errno)));
    }

    return static_cast<uint64_t>(res);
  }
}

File::operator bool() const { return fd != -1; }

void read_all(File &file, uint8_t *data, uint64_t length) {
  while (length > 0) {
    const uint64_t consumed = file.read(data, length);
    if (consumed == 0) {
      throw std::runtime_error("File is too short");
    }

    assert(consumed <= length);
    data += consumed;
    length -= consumed;
  }
}

void write_all(File &file, const uint8_t *data, uint64_t length) {
  while (length > 0) {
    const uint64_t consumed = file.write(data, length);
    if (consumed == 0) {
      throw std::runtime_error(fmt::format("Failed to write {} bytes", length));
    }

    assert(consumed <= length);
    data += consumed;
    length -= consumed;
  }
}

std::vector<uint8_t> read_file(const char *path, Directory *dir) {
  File f(path, File::OpenMode::Read, dir);
  const FileInfo info = f.info();

  std::vector<uint8_t> result(info.size);
  if (info.size > 0) {
    read_all(f, result.data(), result.size());
  }
  return result;
}

void write_file(const char *path, const uint8_t *data, uint64_t length,
                Directory *dir) {
  File f(path,
         core::Bitmask<File::OpenMode>(File::OpenMode::Write,
                                       File::OpenMode::Create,
                                       File::OpenMode::Truncate),
         dir);
  if (length > 0) {
    write_all(f, data, length);
  }
}

} // namespace freeisle::fs
