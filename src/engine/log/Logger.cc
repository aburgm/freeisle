#include "log/Logger.hh"

#include "log/System.hh"

namespace freeisle::log {

Logger::Logger(System &system, Sink &sink, const std::string &domain,
               Level threshold)
    : system_(system), sink_(sink), domain_(domain), threshold_(threshold) {}

Logger Logger::make_child_logger(const std::string &subdomain) const {
  return system_.make_logger(domain_ + "." + subdomain, sink_);
}

Level Logger::threshold() const { return threshold_; }

void Logger::log(Level level, const std::string &message) {
  if (level <= threshold_) {
    const time::Instant time = system_.clock().get_time();
    sink_.log(time, level, domain_, message);
  }
}

} // namespace freeisle::log
