#include "log/test/util/System.hh"

#include "log/Sink.hh"
#include "log/System.hh"
#include "time/Clock.hh"

#include <gtest/gtest.h>

TEST(System, Simple) {
  freeisle::log::test::System test("spectacle");
  test.logger.log(freeisle::log::Level::Info, "my message");

  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(test.sink.domain_, "spectacle");
  EXPECT_EQ(test.sink.message_, "my message");
}

TEST(System, Format) {
  freeisle::log::test::System test("spectacle");
  test.logger.info("test message with {} goodies", 5);

  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(test.sink.domain_, "spectacle");
  EXPECT_EQ(test.sink.message_, "test message with 5 goodies");
}

TEST(System, ChildLogger) {
  freeisle::log::test::System test("spectacle");
  freeisle::log::Logger child_logger = test.logger.make_child_logger("rock");
  child_logger.info("test message");

  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(test.sink.domain_, "spectacle.rock");
  EXPECT_EQ(test.sink.message_, "test message");
}

TEST(System, ThresholdConfig) {
  freeisle::log::test::System test(
      "meh", "warning,spectacle=info,spectacle.=debug,spectacle.rock=error");

  freeisle::log::Logger superbunny_logger =
      test.system.make_logger("superbunny", test.sink);
  freeisle::log::Logger spectacle_logger =
      test.system.make_logger("spectacle", test.sink);
  freeisle::log::Logger spectacle_mountain_logger =
      spectacle_logger.make_child_logger("mountain");
  freeisle::log::Logger spectacle_rock_logger =
      spectacle_logger.make_child_logger("rock");

  EXPECT_EQ(superbunny_logger.threshold(), freeisle::log::Level::Warning);
  EXPECT_EQ(spectacle_logger.threshold(), freeisle::log::Level::Info);
  EXPECT_EQ(spectacle_mountain_logger.threshold(), freeisle::log::Level::Debug);
  EXPECT_EQ(spectacle_rock_logger.threshold(), freeisle::log::Level::Error);

  superbunny_logger.warning("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Warning);
  EXPECT_EQ(test.sink.domain_, "superbunny");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  superbunny_logger.info("test message");
  ASSERT_FALSE(test.sink.called_);
  test.sink = freeisle::log::test::MockSink{};

  spectacle_logger.warning("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Warning);
  EXPECT_EQ(test.sink.domain_, "spectacle");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_logger.info("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(test.sink.domain_, "spectacle");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_logger.debug("test message");
  ASSERT_FALSE(test.sink.called_);
  test.sink = freeisle::log::test::MockSink{};

  spectacle_mountain_logger.info("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Info);
  EXPECT_EQ(test.sink.domain_, "spectacle.mountain");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_mountain_logger.debug("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Debug);
  EXPECT_EQ(test.sink.domain_, "spectacle.mountain");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_rock_logger.fatal("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Fatal);
  EXPECT_EQ(test.sink.domain_, "spectacle.rock");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_rock_logger.error("test message");
  ASSERT_TRUE(test.sink.called_);
  EXPECT_EQ(test.sink.instant_, freeisle::time::Instant::unix_sec(1621371327));
  EXPECT_EQ(test.sink.level_, freeisle::log::Level::Error);
  EXPECT_EQ(test.sink.domain_, "spectacle.rock");
  EXPECT_EQ(test.sink.message_, "test message");
  test.sink = freeisle::log::test::MockSink{};

  spectacle_rock_logger.warning("test message");
  ASSERT_FALSE(test.sink.called_);
  test.sink = freeisle::log::test::MockSink{};
}

TEST(System, Config) {
  freeisle::log::test::MockClock clock;
  EXPECT_NO_THROW(freeisle::log::System(clock, "fatal"));
  EXPECT_NO_THROW(freeisle::log::System(clock, "warning,sepia=error"));
  EXPECT_THROW(freeisle::log::System(clock, "something"),
               std::invalid_argument);
  EXPECT_THROW(freeisle::log::System(clock, "warning,sepia=nonexisting"),
               std::invalid_argument);
}
