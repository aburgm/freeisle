#pragma once

#include "fs/Directory.hh"
#include "fs/FileInfo.hh"

#include "core/Bitmask.hh"

#include <cstdint>
#include <vector>

namespace freeisle::fs {

/**
 * File represents an open file and can be used for reading or writing.
 */
class File {
public:
  /**
   * Specifies different modes with which the file can be opened.
   */
  enum class OpenMode {
    /**
     * Open the file for reading.
     */
    Read,

    /**
     * Open the file for writing.
     */
    Write,

    /**
     * Create the file if it does not exist already.
     */
    Create,

    /**
     * Truncate the file to zero length. This mode can only be
     * set if Write is also set.
     */
    Truncate,

    /**
     * Fail opening the fail if it exists already. This mode can only
     * be set if Create is also set.
     */
    FailIfExist,
  };

  /**
   * The default constructor creates an unopened file.
   */
  File();

  /**
   * Open a file at the given path with the given mode. Throws
   * std::runtime_error if the file could not be opened.
   * @param path Path in the filesystem to the file to be opened.
   * @param mode Mode in which to open the file.
   * @param dir  If path is relative, interpret it relative to this
   *             directory, or to the CWD of the process if null.
   */
  File(const char *path, core::Bitmask<OpenMode> mode, Directory *dir);
  ~File();

  File(const File &) = delete;
  File(File &&other);

  File &operator=(const File &) = delete;
  File &operator=(File &&other);

  bool operator==(const File &) = delete;

  /**
   * Returns information for the opened file.
   * Throws std::runtime_error if the information cannot
   * be obtained.
   */
  FileInfo info() const;

  /**
   * Read data from the file. Throws std::runtime_error if the data
   * cannot be read.
   * @return The number of bytes actually read. Returns 0 on end of file.
   */
  uint64_t read(uint8_t *data, uint64_t length);

  /**
   * Write data to the file. Throws std::runtime_error if the data
   * cannot be written.
   * @return The number of bytes actually written. May be less than length.
   */
  uint64_t write(const uint8_t *data, uint64_t length);

  /**
   * Returns whether the file is open or not.
   */
  explicit operator bool() const;

private:
  int fd;
};

/**
 * Read the given number of bytes from the file. Throws
 * std::runtime_error if the file is not at least this long.
 */
void read_all(File &file, uint8_t *data, uint64_t length);

/**
 * Read the given number of bytes from the file. Throws
 * std::runtime_error if not all data can be written to the file.
 */
void write_all(File &file, const uint8_t *data, uint64_t length);

/**
 * Utility function to read a file into memory in one chunk.
 */
std::vector<uint8_t> read_file(const char *path, Directory *dir);

/**
 * Utility function to write out a portion of memory to a file in
 * a single function call.
 */
void write_file(const char *path, const uint8_t *data, size_t len,
                Directory *dir);

} // namespace freeisle::fs
