#pragma once

#include <string>

namespace freeisle::state {

/**
 * A team is a group of players that play as allies and who
 * win together (but not necessarily lose together).
 */
struct Team {
  /**
   * The name of the team.
   */
  std::string name;
};

} // namespace freeisle::state
