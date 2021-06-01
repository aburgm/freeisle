#pragma once

#include "log/Logger.hh"

#include <map>
#include <string>

namespace freeisle::state::serialize {

struct AuxData {
  log::Logger &logger;
  std::map<const void *, std::string> &object_ids_;
};

} // namespace freeisle::state::serialize
