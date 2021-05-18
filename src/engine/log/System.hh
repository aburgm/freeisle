#pragma once

#include "log/Logger.hh"
#include "log/Sink.hh"

#include "time/Clock.hh"

#include <map>
#include <string>

namespace freeisle::log {

/**
 * A log system is used to create loggers.
 */
class System {
public:
  /**
   * Create a new log system. This is typically a Singleton, but this is
   * not enforced.
   *
   * @param clock  Clock for timekeeping.
   * @param config A string describing logging config, in the form
   *               "<level>,<domain1>=<level1>;<domain2>=<level2>", etc,
   *               specifying thresholds for various logging domains.
   *               If a domain ends in a dot, it applies to all subdomains
   *               as well.
   */
  System(time::Clock &clock, const std::string &config);

  /**
   * Create a new logger with the given domain and sink. The system
   * must outlive the logger that it creates (and all its child loggers).
   */
  Logger make_logger(const std::string &domain, Sink &sink);

  /**
   * Return the logger's clock.
   */
  time::Clock &clock() { return clock_; }

private:
  time::Clock &clock_;
  const std::map<std::string, Level> thresholds_;
};

} // namespace freeisle::log
