#include "fs/test/util/TempDirFixture.hh"

#include "fs/File.hh"

#include <gtest/gtest.h>

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {

// Mock errors from ::read or ::write system calls.
void set_next_read_error(int err) __attribute__((weak));
void set_next_write_error(int err) __attribute__((weak));

void set_next_write_length_to_zero() __attribute__((weak));
}

namespace {

class FileTest : public freeisle::fs::test::TempDirFixture {};

void write_file(const char *path, const char *content) {
  const int fd = ::open(path, O_WRONLY | O_TRUNC | O_CREAT, 0666);
  if (fd == -1) {
    throw std::runtime_error("open");
  }

  const size_t len = ::strlen(content);
  const ssize_t written = ::write(fd, content, len);
  if (written < 0 || static_cast<size_t>(written) != len) {
    throw std::runtime_error("write");
  }

  if (::close(fd) != 0) {
    throw std::runtime_error("close");
  }
}

std::string read_file(const char *path) {
  const int fd = ::open(path, O_RDONLY, 0666);
  if (fd == -1) {
    throw std::runtime_error("open");
  }

  struct stat sbuf;
  if (::fstat(fd, &sbuf) != 0) {
    throw std::runtime_error("fstat");
  }

  std::string str;
  str.resize(sbuf.st_size);
  if (::read(fd, &str[0], sbuf.st_size) != sbuf.st_size) {
    throw std::runtime_error("read");
  }

  if (::close(fd) != 0) {
    throw std::runtime_error("close");
  }

  return str;
}

} // namespace

TEST_F(FileTest, Default) {
  freeisle::fs::File file;
  EXPECT_FALSE(file);
}

TEST_F(FileTest, OpenNonexisting) {
  EXPECT_THROW(freeisle::fs::File("nonexisting.file",
                                  freeisle::fs::File::OpenMode::Read, nullptr),
               std::runtime_error);
  EXPECT_THROW(freeisle::fs::File("nonexisting.file",
                                  freeisle::fs::File::OpenMode::Write, nullptr),
               std::runtime_error);

  freeisle::fs::File f("nonexisting.file", freeisle::fs::File::OpenMode::Create,
                       nullptr);
  EXPECT_EQ(f.info().size, 0);
}

TEST_F(FileTest, OpenRead) {
  write_file("bla.txt", "hi");

  freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read, nullptr);
  ASSERT_TRUE(f);

  const freeisle::fs::FileInfo info = f.info();
  ASSERT_EQ(info.size, 2);

  uint8_t buf[16];
  ASSERT_EQ(f.read(buf, 16), 2);
  ASSERT_EQ(std::string(buf, buf + 2), "hi");

  // File is opened readonly:
  EXPECT_THROW(f.write(buf, 2), std::runtime_error);
}

TEST_F(FileTest, OpenWithDirectory) {
  ASSERT_EQ(::mkdir("dir", 0755), 0);

  write_file("dir/bla.txt", "hi");

  freeisle::fs::Directory dir("dir", nullptr);
  freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read, &dir);

  ASSERT_EQ(f.info().size, 2);
}

TEST_F(FileTest, OpenReadWrite) {
  write_file("bla.txt", "hi");

  freeisle::fs::File f("bla.txt",
                       freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                           freeisle::fs::File::OpenMode::Read,
                           freeisle::fs::File::OpenMode::Write),
                       nullptr);
  ASSERT_TRUE(f);

  ASSERT_EQ(f.info().size, 2);

  uint8_t buf[2];
  ASSERT_EQ(f.read(buf, 16), 2);
  ASSERT_EQ(std::string(buf, buf + 2), "hi");

  ASSERT_EQ(f.write(reinterpret_cast<const uint8_t *>("oops"), 4), 4);
  ASSERT_EQ(f.info().size, 6);

  ASSERT_EQ(read_file("bla.txt"), "hioops");
}

TEST_F(FileTest, OpenCreate) {
  {
    freeisle::fs::File f("bla.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write,
                             freeisle::fs::File::OpenMode::Create),
                         nullptr);
    ASSERT_EQ(f.write(reinterpret_cast<const uint8_t *>("oops"), 4), 4);

    // File is opened write-only:
    uint8_t buf[2];
    EXPECT_THROW(f.read(buf, 2), std::runtime_error);
  }

  ASSERT_EQ(read_file("bla.txt"), "oops");

  EXPECT_THROW(freeisle::fs::File f(
                   "bla.txt",
                   freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                       freeisle::fs::File::OpenMode::Write,
                       freeisle::fs::File::OpenMode::Create,
                       freeisle::fs::File::OpenMode::FailIfExist),
                   nullptr),
               std::runtime_error);

  {
    freeisle::fs::File f("bla2.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write,
                             freeisle::fs::File::OpenMode::Create,
                             freeisle::fs::File::OpenMode::FailIfExist),
                         nullptr);
    ASSERT_TRUE(f);
    ASSERT_EQ(f.write(reinterpret_cast<const uint8_t *>("bla"), 3), 3);
  }

  ASSERT_EQ(read_file("bla2.txt"), "bla");

  {
    freeisle::fs::File f("bla2.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write),
                         nullptr);
    ASSERT_EQ(f.info().size, 3);

    ASSERT_EQ(f.write(reinterpret_cast<const uint8_t *>("well"), 4), 4);
    ASSERT_EQ(f.info().size, 4);
  }

  {
    freeisle::fs::File f("bla2.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write,
                             freeisle::fs::File::OpenMode::Truncate),
                         nullptr);
    ASSERT_EQ(f.info().size, 0);
    ASSERT_EQ(f.write(reinterpret_cast<const uint8_t *>("flute"), 5), 5);
    ASSERT_EQ(f.info().size, 5);
  }
}

TEST_F(FileTest, FileInfo) {
  write_file("bla1.txt", "hi");
  write_file("bla2.txt", "hi2");

  // hardlink: we expect same FileId:
  ASSERT_EQ(::link("bla2.txt", "bla3.txt"), 0);

  freeisle::fs::File f1("bla1.txt", freeisle::fs::File::OpenMode::Read,
                        nullptr);
  freeisle::fs::File f2("bla2.txt", freeisle::fs::File::OpenMode::Read,
                        nullptr);
  freeisle::fs::File f3("bla3.txt", freeisle::fs::File::OpenMode::Read,
                        nullptr);
  freeisle::fs::File f1_2("bla1.txt", freeisle::fs::File::OpenMode::Read,
                          nullptr);

  ASSERT_TRUE(f1);
  ASSERT_TRUE(f2);
  ASSERT_TRUE(f3);
  ASSERT_TRUE(f1_2);

  const freeisle::fs::FileInfo i1 = f1.info();
  const freeisle::fs::FileInfo i2 = f2.info();
  const freeisle::fs::FileInfo i3 = f3.info();
  const freeisle::fs::FileInfo i1_2 = f1_2.info();

  EXPECT_EQ(i1.size, 2);
  EXPECT_EQ(i2.size, 3);
  EXPECT_EQ(i3.size, 3);
  EXPECT_EQ(i1_2.size, 2);

  EXPECT_EQ(i1.id, i1_2.id);
  EXPECT_NE(i1.id, i2.id);
  EXPECT_NE(i1.id, i3.id);

  EXPECT_EQ(i2.id, i3.id);
  EXPECT_NE(i2.id, i1_2.id);

  EXPECT_NE(i3.id, i1_2.id);

  EXPECT_TRUE(i1.id < i2.id || i2.id < i1.id);
  EXPECT_FALSE(i1.id < i2.id && i2.id < i1.id);
}

TEST_F(FileTest, MoveCtorAndAssignment) {
  write_file("bla.txt", "hi");

  freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read, nullptr);
  ASSERT_TRUE(f);

  freeisle::fs::File f2(std::move(f));
  ASSERT_TRUE(f2);
  ASSERT_FALSE(f);

  ASSERT_EQ(f2.info().size, 2);
  uint8_t buf[4];
  ASSERT_EQ(f2.read(buf, 4), 2);
  ASSERT_EQ(std::string(buf, buf + 2), "hi");

  freeisle::fs::File f3;
  f3 = std::move(f2);
  ASSERT_TRUE(f3);
  ASSERT_FALSE(f2);
  ASSERT_FALSE(f);

  f3 = std::move(f3);
  ASSERT_TRUE(f3);

  write_file("bla2.txt", "hi2");
  freeisle::fs::File f4("bla2.txt", freeisle::fs::File::OpenMode::Read,
                        nullptr);

  f3 = std::move(f4);
  ASSERT_TRUE(f3);
  ASSERT_FALSE(f4);

  ASSERT_EQ(f3.read(buf, 4), 3);
  ASSERT_EQ(std::string(buf, buf + 3), "hi2");
}

TEST_F(FileTest, OpsOnClosedFile) {
  freeisle::fs::File f;
  EXPECT_THROW(f.info(), std::exception);

  uint8_t buf[16];
  EXPECT_THROW(f.read(buf, 16), std::exception);
  EXPECT_THROW(f.write(buf, 16), std::exception);
}

TEST_F(FileTest, Eof) {
  write_file("bla.txt", "hi");
  freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read, nullptr);

  uint8_t buf[4];
  ASSERT_EQ(f.read(buf, 4), 2);
  ASSERT_EQ(std::string(buf, buf + 2), "hi");

  ASSERT_EQ(f.read(buf, 4), 0);
  ASSERT_EQ(f.read(buf, 4), 0);
  ASSERT_EQ(f.read(buf, 4), 0);
}

TEST_F(FileTest, SystemErrors) {
  write_file("bla.txt", "hi");
  freeisle::fs::File f("bla.txt",
                       freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                           freeisle::fs::File::OpenMode::Read,
                           freeisle::fs::File::OpenMode::Write),
                       nullptr);

  set_next_read_error(EINTR);
  uint8_t buf[4];
  ASSERT_EQ(f.read(buf, 4), 2);
  ASSERT_EQ(std::string(buf, buf + 2), "hi");

  set_next_read_error(EPERM);
  EXPECT_THROW(f.read(buf, 4), std::runtime_error);

  EXPECT_EQ(f.write(buf, 2), 2);

  set_next_write_error(EINTR);
  EXPECT_EQ(f.write(buf, 2), 2);

  set_next_write_error(EINVAL);
  EXPECT_THROW(f.write(buf, 2), std::runtime_error);

  ASSERT_EQ(read_file("bla.txt"), "hihihi");
}

TEST_F(FileTest, ReadAll) {
  write_file("bla.txt", "hi everyone");

  {
    freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read,
                         nullptr);
    uint8_t bla[11];
    freeisle::fs::read_all(f, bla, 11);
    ASSERT_EQ(std::string(bla, bla + 11), "hi everyone");
  }

  {
    freeisle::fs::File f("bla.txt", freeisle::fs::File::OpenMode::Read,
                         nullptr);
    uint8_t bla[12];
    EXPECT_THROW(freeisle::fs::read_all(f, bla, 12), std::runtime_error);
  }
}

TEST_F(FileTest, WriteAll) {
  const char *text = "hi everyone";

  {
    freeisle::fs::File f("bla.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write,
                             freeisle::fs::File::OpenMode::Create),
                         nullptr);
    freeisle::fs::write_all(f, reinterpret_cast<const uint8_t *>(text),
                            ::strlen(text));

    ASSERT_EQ(read_file("bla.txt"), "hi everyone");
  }

  {
    freeisle::fs::File f("bla2.txt",
                         freeisle::core::Bitmask<freeisle::fs::File::OpenMode>(
                             freeisle::fs::File::OpenMode::Write,
                             freeisle::fs::File::OpenMode::Create),
                         nullptr);

    // I don't think we can have write return 0 without failing or
    // setting errno, so we need to mock this behavior to test test
    // the code that handles it.
    set_next_write_length_to_zero();
    EXPECT_THROW(
        freeisle::fs::write_all(f, reinterpret_cast<const uint8_t *>(text),
                                ::strlen(text)),
        std::runtime_error);
  }
}

TEST_F(FileTest, ReadFile) {
  write_file("bla.txt", "hi everyone");
  write_file("empty.txt", "");
  ASSERT_EQ(mkdir("dir", 0755), 0);
  write_file("dir/child.txt", "icerod");

  freeisle::fs::Directory dir("dir", nullptr);

  const std::vector<uint8_t> data1 =
      freeisle::fs::read_file("bla.txt", nullptr);
  const std::vector<uint8_t> data2 =
      freeisle::fs::read_file("empty.txt", nullptr);
  const std::vector<uint8_t> data3 =
      freeisle::fs::read_file("dir/child.txt", nullptr);
  const std::vector<uint8_t> data4 = freeisle::fs::read_file("child.txt", &dir);
  EXPECT_THROW(freeisle::fs::read_file("nonexisting.txt", nullptr),
               std::runtime_error);
  EXPECT_THROW(freeisle::fs::read_file("nonexisting.txt", &dir),
               std::runtime_error);

  ASSERT_EQ(data1.size(), 11);
  ASSERT_EQ(data2.size(), 0);
  ASSERT_EQ(data3.size(), 6);
  ASSERT_EQ(data4.size(), 6);

  ASSERT_EQ(::memcmp(data1.data(), "hi everyone", 11), 0);
  ASSERT_EQ(::memcmp(data3.data(), "icerod", 6), 0);
  ASSERT_EQ(::memcmp(data4.data(), "icerod", 6), 0);
}

TEST_F(FileTest, WriteFile) {
  ASSERT_EQ(mkdir("dir", 0755), 0);
  freeisle::fs::Directory dir("dir", nullptr);

  freeisle::fs::write_file(
      "bla.txt", reinterpret_cast<const uint8_t *>("hi everyone"), 11, nullptr);
  freeisle::fs::write_file("empty.txt", reinterpret_cast<const uint8_t *>(""),
                           0, nullptr);
  freeisle::fs::write_file("dir/child.txt",
                           reinterpret_cast<const uint8_t *>("firerod"), 7,
                           nullptr);
  freeisle::fs::write_file(
      "child2.txt", reinterpret_cast<const uint8_t *>("icerod"), 6, &dir);
  EXPECT_THROW(freeisle::fs::write_file(
                   "nonexisting/something.txt",
                   reinterpret_cast<const uint8_t *>("cape"), 4, nullptr),
               std::runtime_error);

  ASSERT_EQ(read_file("bla.txt"), "hi everyone");
  ASSERT_EQ(read_file("empty.txt"), "");
  ASSERT_EQ(read_file("dir/child.txt"), "firerod");
  ASSERT_EQ(read_file("dir/child2.txt"), "icerod");
}
