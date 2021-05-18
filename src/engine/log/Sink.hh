#pragma once

#include "time/Instant.hh"

#include <string>

namespace freeisle::log {

/**
 * Specifies different severity levels for logging.
 */
enum class Level {
  Fatal,
  Error,
  Warning,
  Info,
  Debug,
};

/**
 * Sink for a log message: a sink specifies where the logged data
 * eventually ends up.
 */
class Sink {
public:
  virtual ~Sink() = default;

  /**
   * Log the given message.
   * @param instant The time at which the log message was generated.
   * @param level   Severity of the message.
   * @param domain  Domain or module where the message was generated from.
   * @param message The log message text.
   */
  virtual void log(time::Instant instant, Level level,
                   const std::string &domain, const std::string &message) = 0;
};

} // namespace freeisle::log
