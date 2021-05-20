#pragma once

namespace freeisle::fs {

/**
 * Directory represents a possibly open directory.
 */
class Directory {
  friend class File;

public:
  /**
   * The default constructor constructs an unopened directory.
   */
  Directory();
  ~Directory();

  /**
   * Construct a directory from a path on the filesystem. Throws
   * std::runtime_error if the directory at the given path cannot
   * be opened.
   *
   * @param path Path to the directory to open on the filesystem.
   * @param dir  If path is relative, look it up relative to this
   *             directory. Can be null, in which case the path
   *             is looked up relative to the CWD of the process.
   */
  Directory(const char *path, Directory *dir);

  Directory(const Directory &) = delete;
  Directory(Directory &&other);

  Directory &operator=(const Directory &) = delete;
  Directory &operator=(Directory &&other);

  bool operator==(const Directory &) = delete;

  /**
   * Returns whether the directory is open or not.
   */
  explicit operator bool() const;

private:
  int fd;
};

} // namespace freeisle::fs
