#include "log/Sink.hh"
#include "log/System.hh"
#include "time/Clock.hh"

#include <gtest/gtest.h>

namespace {

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

} // namespace

TEST(System, Simple) {
  MockClock clock;
  MockSink sink;
  freeisle::log::System system(clock, "");

  freeisle::log::Logger logger = system.make_logger("spectacle", sink);
  logger.log(freeisle::log::Level::Info, "my message");

  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(sink.domain_, "spectacle");
  EXPECT_EQ(sink.message_, "my message");
}

TEST(System, Format) {
  MockClock clock;
  MockSink sink;
  freeisle::log::System system(clock, "");

  freeisle::log::Logger logger = system.make_logger("spectacle", sink);
  logger.info("test message with {} goodies", 5);

  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(sink.domain_, "spectacle");
  EXPECT_EQ(sink.message_, "test message with 5 goodies");
}

TEST(System, ChildLogger) {
  MockClock clock;
  MockSink sink;
  freeisle::log::System system(clock, "");

  freeisle::log::Logger logger = system.make_logger("spectacle", sink);
  freeisle::log::Logger child_logger = logger.make_child_logger("rock");
  child_logger.info("test message");

  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(sink.domain_, "spectacle.rock");
  EXPECT_EQ(sink.message_, "test message");
}

TEST(System, ThresholdConfig) {
  MockClock clock;
  MockSink sink;
  freeisle::log::System system(
      clock, "warning,spectacle=info,spectacle.=debug,spectacle.rock=error");

  freeisle::log::Logger superbunny_logger =
      system.make_logger("superbunny", sink);
  freeisle::log::Logger spectacle_logger =
      system.make_logger("spectacle", sink);
  freeisle::log::Logger spectacle_mountain_logger =
      spectacle_logger.make_child_logger("mountain");
  freeisle::log::Logger spectacle_rock_logger =
      spectacle_logger.make_child_logger("rock");

  EXPECT_EQ(superbunny_logger.threshold(), freeisle::log::Level::Warning);
  EXPECT_EQ(spectacle_logger.threshold(), freeisle::log::Level::Info);
  EXPECT_EQ(spectacle_mountain_logger.threshold(), freeisle::log::Level::Debug);
  EXPECT_EQ(spectacle_rock_logger.threshold(), freeisle::log::Level::Error);

  superbunny_logger.warning("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Warning);
  EXPECT_EQ(sink.domain_, "superbunny");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  superbunny_logger.info("test message");
  ASSERT_FALSE(sink.called_);
  sink = MockSink{};

  spectacle_logger.warning("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Warning);
  EXPECT_EQ(sink.domain_, "spectacle");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_logger.info("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(sink.domain_, "spectacle");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_logger.debug("test message");
  ASSERT_FALSE(sink.called_);
  sink = MockSink{};

  spectacle_mountain_logger.info("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(sink.domain_, "spectacle.mountain");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_mountain_logger.debug("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Debug);
  EXPECT_EQ(sink.domain_, "spectacle.mountain");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_rock_logger.fatal("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Fatal);
  EXPECT_EQ(sink.domain_, "spectacle.rock");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_rock_logger.error("test message");
  ASSERT_TRUE(sink.called_);
  EXPECT_EQ(sink.instant_, freeisle::time::Instant::unixSec(1621371327));
  EXPECT_EQ(sink.level_, freeisle::log::Level::Error);
  EXPECT_EQ(sink.domain_, "spectacle.rock");
  EXPECT_EQ(sink.message_, "test message");
  sink = MockSink{};

  spectacle_rock_logger.warning("test message");
  ASSERT_FALSE(sink.called_);
  sink = MockSink{};
}

TEST(System, Config) {
  MockClock clock;
  MockSink sink;
  EXPECT_NO_THROW(freeisle::log::System(clock, "fatal"));
  EXPECT_NO_THROW(freeisle::log::System(clock, "warning,sepia=error"));
  EXPECT_THROW(freeisle::log::System(clock, "something"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::log::System(clock, "warning,sepia=nonexisting"),
               std::invalid_argument);
}
