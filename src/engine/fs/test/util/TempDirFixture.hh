#include <gtest/gtest.h>

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace freeisle::fs::test {

class TempDirFixture : public ::testing::Test {
public:
  virtual void SetUp() override {
    char templ[] = "/tmp/testXXXXXX";
    ASSERT_NE(::mkdtemp(templ), nullptr);

    char cwd[256];
    ASSERT_NE(::getcwd(cwd, sizeof(cwd)), nullptr);

    // TODO(armin): open the origDirectory and use fchdir on return.
    origDirectory = cwd;
    directory = templ;

    ASSERT_EQ(::chdir(templ), 0);
  }

  virtual void TearDown() override {
    ASSERT_EQ(::chdir(origDirectory.c_str()), 0);
    delete_directory(directory.c_str(), AT_FDCWD);
  }

  void delete_directory(const char *path, int dirfd) {
    const int fd = ::openat(dirfd, path, O_DIRECTORY);
    ASSERT_NE(fd, -1) << std::strerror(errno);

    DIR *dir = ::fdopendir(fd);
    ASSERT_NE(dir, nullptr);

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      if (entry->d_type == DT_DIR) {
        delete_directory(entry->d_name, ::dirfd(dir));
      } else {
        ASSERT_EQ(::unlinkat(::dirfd(dir), entry->d_name, 0), 0);
      }
    }

    ASSERT_EQ(::closedir(dir), 0);
    ASSERT_EQ(::unlinkat(dirfd, path, AT_REMOVEDIR), 0);
  }

  std::string origDirectory;
  std::string directory;
};

} // namespace freeisle::fs::test
