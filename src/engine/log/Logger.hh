#pragma once

#include "log/Sink.hh"

#include <fmt/format.h>

#include <string>
#include <utility>

namespace freeisle::log {

class System;

/**
 * A logger is an object through which log messages can be written.
 * Loggers cannot be instantiated directly, but must be created through
 * a log system, or as a child of an existing logger.
 */
class Logger {
  friend class System;

public:
  Logger() = delete;
  Logger(const Logger &) = delete;
  Logger(Logger &&) = default;
  Logger &operator=(const Logger &) = delete;
  Logger &operator=(Logger &&) = delete;

  /**
   * Creates a new logger which has the given subdomain appended
   * to this logger's domain, e.g. "freeisle.ai".
   */
  Logger make_child_logger(const std::string &subdomain) const;

  /**
   * Returns the threshold of this logger. Log messages with severity lower
   * than this are not going to be logged, so e.g. if the threshold is warning,
   * then info and debug messages are not being logged.
   */
  Level threshold() const;

  /**
   * Log the given message at the given severity level.
   */
  void log(Level level, const std::string &message);

  /**
   * Log and format the message at the given severity level.
   */
  template <typename... T>
  void log(Level level, const std::string &message, T &&... args) {
    log(level, fmt::format(message, std::forward(args)...));
  }

  /**
   * Log and format the given message at fatal level.
   */
  template <typename... T>
  void fatal(const std::string &message, T &&... args) {
    log(Level::Fatal, fmt::format(message, std::forward<T>(args)...));
  }

  /**
   * Log and format the given message at error level.
   */
  template <typename... T>
  void error(const std::string &message, T &&... args) {
    log(Level::Error, fmt::format(message, std::forward<T>(args)...));
  }

  /**
   * Log and format the given message at warning level.
   */
  template <typename... T>
  void warning(const std::string &message, T &&... args) {
    log(Level::Warning, fmt::format(message, std::forward<T>(args)...));
  }

  /**
   * Log and format the given message at info level.
   */
  template <typename... T> void info(const std::string &message, T &&... args) {
    log(Level::Info, fmt::format(message, std::forward<T>(args)...));
  }

  /**
   * Log and format the given message at debug level.
   */
  template <typename... T>
  void debug(const std::string &message, T &&... args) {
    log(Level::Debug, fmt::format(message, std::forward<T>(args)...));
  }

private:
  explicit Logger(System &system, Sink &sink, const std::string &domain,
                  Level threshold);

  System &system_;
  Sink &sink_;
  const std::string domain_;
  const Level threshold_;
};

} // namespace freeisle::log
