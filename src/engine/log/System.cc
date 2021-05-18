#include "log/System.hh"

#include "core/Enum.hh"

namespace freeisle::log {

namespace {

const core::EnumEntry<Level> levels[]{
    {Level::Debug, "debug"},     {Level::Info, "info"},
    {Level::Warning, "warning"}, {Level::Error, "error"},
    {Level::Fatal, "fatal"},
};

void add_threshold(std::map<std::string, Level> &map,
                   const std::string &fragment) {
  if (fragment.empty()) {
    return;
  }

  std::string frag_domain;
  std::string frag_level;

  std::string::size_type pos = fragment.find('=');
  if (pos != std::string::npos) {
    frag_domain = fragment.substr(0, pos);
    frag_level = fragment.substr(pos + 1);
  } else {
    frag_level = fragment;
  }

  const Level *level = core::from_string(levels, frag_level.c_str());
  if (level == nullptr) {
    throw std::invalid_argument("No such log level: " + frag_level);
  }

  map[frag_domain] = *level;
}

std::map<std::string, Level> make_threshold_map(const std::string &config) {
  std::map<std::string, Level> result;

  std::string::size_type prev = 0;
  while (prev != std::string::npos) {
    const std::string::size_type pos = config.find(',', prev);

    if (pos == std::string::npos) {
      add_threshold(result, config.substr(prev));
      prev = pos;
    } else {
      add_threshold(result, config.substr(prev, pos - prev));
      prev = pos + 1;
    }
  }

  if (result.empty()) {
    result[""] = Level::Info;
  }

  return result;
}

} // namespace

System::System(time::Clock &clock, const std::string &config)
    : clock_(clock), thresholds_(make_threshold_map(config)) {}

Logger System::make_logger(const std::string &domain, Sink &sink) {
  std::string domain_search(domain);
  std::map<std::string, Level>::const_iterator iter =
      thresholds_.find(domain_search);
  while (iter == thresholds_.end()) {
    std::string::size_type pos = domain_search.rfind('.');

    if (pos != std::string::npos) {
      domain_search.erase(pos + 1);
      iter = thresholds_.find(domain_search);
      domain_search.erase(pos);
    } else {
      iter = thresholds_.find("");
    }
  }

  return Logger(*this, sink, domain, iter->second);
}

} // namespace freeisle::log
