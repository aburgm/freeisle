#include "fs/FileInfo.hh"

namespace freeisle::fs {

FileId::FileId(uint64_t dev, uint64_t ino) : dev_(dev), ino_(ino) {}

bool FileId::operator==(FileId rhs) const {
  return dev_ == rhs.dev_ && ino_ == rhs.ino_;
}

bool FileId::operator!=(FileId rhs) const { return !(*this == rhs); }

bool FileId::operator<(FileId rhs) const {
  return dev_ < rhs.dev_ || (dev_ == rhs.dev_ && ino_ < rhs.ino_);
}

} // namespace freeisle::fs
