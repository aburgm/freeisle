#include "png/Png.hh"

#include "fs/File.hh"
#include "log/System.hh"
#include "time/Clock.hh"

#include <gtest/gtest.h>

extern "C" {

void set_create_read_struct_fail() __attribute__((weak));
void set_create_write_struct_fail() __attribute__((weak));
void set_create_info_struct_fail() __attribute__((weak));
void set_create_info_struct_warn() __attribute__((weak));
void set_write_info_fail() __attribute((weak));
}

namespace {

// TODO(armin): provide these in some test utility lib
class MockClock : public freeisle::time::Clock {
public:
  virtual freeisle::time::Instant get_time() override {
    return freeisle::time::Instant::unixSec(1621371327);
  }

  virtual freeisle::time::Duration get_monotonic_time() override {
    return freeisle::time::Duration::sec(0);
  }
};

class MockSink : public freeisle::log::Sink {
public:
  virtual void log(freeisle::time::Instant instant, freeisle::log::Level level,
                   const std::string &domain,
                   const std::string &message) override {
    called_ = true;
    instant_ = instant;
    level_ = level;
    domain_ = domain;
    message_ = message;
  }

  bool called_ = false;
  freeisle::time::Instant instant_;
  freeisle::log::Level level_;
  std::string domain_;
  std::string message_;
};

class PngTest : public ::testing::Test {
public:
  PngTest() : system(clock, ""), logger(system.make_logger("test", sink)) {}

  MockClock clock;
  MockSink sink;

  freeisle::log::System system;
  freeisle::log::Logger logger;
};

class PngDecodeRgb8ColorTest
    : public PngTest,
      public ::testing::WithParamInterface<const char *> {};
class PngDecodeRgb8GreyTest
    : public PngTest,
      public ::testing::WithParamInterface<const char *> {};

} // namespace

TEST_P(PngDecodeRgb8ColorTest, Decode) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file(GetParam(), nullptr);
  const freeisle::core::Grid<freeisle::core::color::Rgb8> image =
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger));

  EXPECT_FALSE(sink.called_);

  ASSERT_EQ(image.width(), 2);
  ASSERT_EQ(image.height(), 2);

  EXPECT_EQ(image(0, 0).r, 255);
  EXPECT_EQ(image(0, 0).g, 0);
  EXPECT_EQ(image(0, 0).b, 0);

  EXPECT_EQ(image(1, 0).r, 161);
  EXPECT_EQ(image(1, 0).g, 99);
  EXPECT_EQ(image(1, 0).b, 197);

  EXPECT_EQ(image(0, 1).r, 255);
  EXPECT_EQ(image(0, 1).g, 255);
  EXPECT_EQ(image(0, 1).b, 255);

  EXPECT_EQ(image(1, 1).r, 18);
  EXPECT_EQ(image(1, 1).g, 255);
  EXPECT_EQ(image(1, 1).b, 0);
}

TEST_P(PngDecodeRgb8GreyTest, Decode) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file(GetParam(), nullptr);
  const freeisle::core::Grid<freeisle::core::color::Rgb8> image =
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger));

  EXPECT_FALSE(sink.called_);

  ASSERT_EQ(image.width(), 2);
  ASSERT_EQ(image.height(), 2);

  EXPECT_EQ(image(0, 0).r, 130);
  EXPECT_EQ(image(0, 0).g, 130);
  EXPECT_EQ(image(0, 0).b, 130);

  EXPECT_EQ(image(1, 0).r, 124);
  EXPECT_EQ(image(1, 0).g, 124);
  EXPECT_EQ(image(1, 0).b, 124);

  EXPECT_EQ(image(0, 1).r, 255);
  EXPECT_EQ(image(0, 1).g, 255);
  EXPECT_EQ(image(0, 1).b, 255);

  EXPECT_EQ(image(1, 1).r, 220);
  EXPECT_EQ(image(1, 1).g, 220);
  EXPECT_EQ(image(1, 1).b, 220);
}

INSTANTIATE_TEST_CASE_P(DecodeRgb8ColorTests, PngDecodeRgb8ColorTest,
                        ::testing::Values("data/rgb8.png", "data/rgb16.png",
                                          "data/rgba8.png",
                                          "data/indexed.png"));

INSTANTIATE_TEST_CASE_P(DecodeRgb8GreyTests, PngDecodeRgb8GreyTest,
                        ::testing::Values("data/gray.png"));

TEST_F(PngTest, DecodeCorrupt) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file("data/corrupt.png", nullptr);

  EXPECT_THROW(
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger)),
      std::runtime_error);

  EXPECT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_.unixSec(), 1621371327);
  EXPECT_EQ(sink.level_, freeisle::log::Level::Error);
  EXPECT_EQ(sink.domain_, "test");
}

TEST_F(PngTest, DecodeReadStructOom) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file("data/rgb8.png", nullptr);
  set_create_read_struct_fail();

  EXPECT_THROW(
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger)),
      std::runtime_error);

  EXPECT_FALSE(sink.called_);
}

TEST_F(PngTest, DecodeInfoStructOom) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file("data/rgb8.png", nullptr);
  set_create_info_struct_fail();

  EXPECT_THROW(
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger)),
      std::runtime_error);

  EXPECT_FALSE(sink.called_);
}

TEST_F(PngTest, DecodeInfoStructWarn) {
  const std::vector<uint8_t> data =
      freeisle::fs::read_file("data/rgb8.png", nullptr);

  set_create_info_struct_warn();

  const freeisle::core::Grid<freeisle::core::color::Rgb8> image =
      freeisle::png::decode_rgb8(data.data(), data.size(), std::move(logger));

  EXPECT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_.unixSec(), 1621371327);
  EXPECT_EQ(sink.level_, freeisle::log::Level::Warning);
  EXPECT_EQ(sink.domain_, "test");
  EXPECT_EQ(sink.message_, "Mock warning");

  ASSERT_EQ(image.width(), 2);
  ASSERT_EQ(image.height(), 2);

  EXPECT_EQ(image(0, 0).r, 255);
  EXPECT_EQ(image(0, 0).g, 0);
  EXPECT_EQ(image(0, 0).b, 0);

  EXPECT_EQ(image(1, 0).r, 161);
  EXPECT_EQ(image(1, 0).g, 99);
  EXPECT_EQ(image(1, 0).b, 197);

  EXPECT_EQ(image(0, 1).r, 255);
  EXPECT_EQ(image(0, 1).g, 255);
  EXPECT_EQ(image(0, 1).b, 255);

  EXPECT_EQ(image(1, 1).r, 18);
  EXPECT_EQ(image(1, 1).g, 255);
  EXPECT_EQ(image(1, 1).b, 0);
}

TEST_F(PngTest, Encode) {
  freeisle::core::Grid<freeisle::core::color::Rgb8> image(2, 2);
  image(0, 0) = freeisle::core::color::Rgb8{255, 0, 0};
  image(1, 0) = freeisle::core::color::Rgb8{0, 0, 0};
  image(0, 1) = freeisle::core::color::Rgb8{127, 127, 0};
  image(1, 1) = freeisle::core::color::Rgb8{255, 0, 127};

  const std::vector<uint8_t> encoded =
      freeisle::png::encode_rgb8(image, logger.make_child_logger("encode"));
  const freeisle::core::Grid<freeisle::core::color::Rgb8> decoded =
      freeisle::png::decode_rgb8(encoded.data(), encoded.size(),
                                 logger.make_child_logger("decode"));

  EXPECT_FALSE(sink.called_);

  ASSERT_EQ(decoded.width(), 2);
  ASSERT_EQ(decoded.height(), 2);

  EXPECT_EQ(decoded(0, 0).r, image(0, 0).r);
  EXPECT_EQ(decoded(0, 0).g, image(0, 0).g);
  EXPECT_EQ(decoded(0, 0).b, image(0, 0).b);

  EXPECT_EQ(decoded(1, 0).r, image(1, 0).r);
  EXPECT_EQ(decoded(1, 0).g, image(1, 0).g);
  EXPECT_EQ(decoded(1, 0).b, image(1, 0).b);

  EXPECT_EQ(decoded(0, 1).r, image(0, 1).r);
  EXPECT_EQ(decoded(0, 1).g, image(0, 1).g);
  EXPECT_EQ(decoded(0, 1).b, image(0, 1).b);

  EXPECT_EQ(decoded(1, 1).r, image(1, 1).r);
  EXPECT_EQ(decoded(1, 1).g, image(1, 1).g);
  EXPECT_EQ(decoded(1, 1).b, image(1, 1).b);
}

TEST_F(PngTest, EncodeWriteStructOom) {
  freeisle::core::Grid<freeisle::core::color::Rgb8> image(2, 2);
  image(0, 0) = freeisle::core::color::Rgb8{255, 0, 0};
  image(1, 0) = freeisle::core::color::Rgb8{0, 0, 0};
  image(0, 1) = freeisle::core::color::Rgb8{127, 127, 0};
  image(1, 1) = freeisle::core::color::Rgb8{255, 0, 127};

  set_create_write_struct_fail();

  EXPECT_THROW(
      freeisle::png::encode_rgb8(image, logger.make_child_logger("encode")),
      std::runtime_error);

  EXPECT_FALSE(sink.called_);
}

TEST_F(PngTest, EncodeInfoStructOom) {
  freeisle::core::Grid<freeisle::core::color::Rgb8> image(2, 2);
  image(0, 0) = freeisle::core::color::Rgb8{255, 0, 0};
  image(1, 0) = freeisle::core::color::Rgb8{0, 0, 0};
  image(0, 1) = freeisle::core::color::Rgb8{127, 127, 0};
  image(1, 1) = freeisle::core::color::Rgb8{255, 0, 127};

  set_create_info_struct_fail();

  EXPECT_THROW(
      freeisle::png::encode_rgb8(image, logger.make_child_logger("encode")),
      std::runtime_error);

  EXPECT_FALSE(sink.called_);
}

TEST_F(PngTest, EncodeWriteFailure) {
  freeisle::core::Grid<freeisle::core::color::Rgb8> image(2, 2);
  image(0, 0) = freeisle::core::color::Rgb8{255, 0, 0};
  image(1, 0) = freeisle::core::color::Rgb8{0, 0, 0};
  image(0, 1) = freeisle::core::color::Rgb8{127, 127, 0};
  image(1, 1) = freeisle::core::color::Rgb8{255, 0, 127};

  set_write_info_fail();

  EXPECT_THROW(
      freeisle::png::encode_rgb8(image, logger.make_child_logger("encode")),
      std::runtime_error);

  EXPECT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_.unixSec(), 1621371327);
  EXPECT_EQ(sink.level_, freeisle::log::Level::Error);
  EXPECT_EQ(sink.domain_, "test.encode");
  EXPECT_EQ(sink.message_, "Mock error");
}
