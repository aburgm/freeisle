#pragma once

#include "time/Clock.hh"
#include "time/Duration.hh"
#include "time/Instant.hh"

#include "log/Sink.hh"
#include "log/System.hh"

namespace freeisle::log::test {

class MockClock : public freeisle::time::Clock {
public:
  virtual freeisle::time::Instant get_time() override {
    return freeisle::time::Instant::unix_sec(1621371327);
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

struct System {
  System(const std::string &name = "test", const std::string &config = "")
      : system(clock, config), logger(system.make_logger(name, sink)) {}

  MockClock clock;
  MockSink sink;
  log::System system;
  log::Logger logger;
};

} // namespace freeisle::log::test
