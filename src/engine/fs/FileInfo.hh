#pragma once

#include <cstdint>

namespace freeisle::fs {

/**
 * Unique identifier for a file. The implementation is opaque, but FileIds
 * can be compared and sorted.
 */
class FileId {
  friend class File;

public:
  FileId() = default;
  FileId(const FileId &) = default;
  FileId(FileId &&) = default;
  FileId &operator=(const FileId &) = default;
  FileId &operator=(FileId &&) = default;

  bool operator==(FileId rhs) const;
  bool operator!=(FileId rhs) const;
  bool operator<(FileId rhs) const;

  explicit operator bool() const;

private:
  FileId(uint64_t dev, uint64_t ino);

  uint64_t dev_;
  uint64_t ino_;
};

/**
 * Metadata about a file.
 */
struct FileInfo {
  /**
   * Unique file ID.
   */
  FileId id;

  /**
   * File size, in bytes.
   */
  uint64_t size;
};

} // namespace freeisle::fs
