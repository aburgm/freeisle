#pragma once

#include <string>

namespace freeisle::def {

/**
 * Specifies a decoration on the map.
 *
 * Other than the name there is not much here as it does not have any impact on
 * the game logic or state. It is meant only for visuals, so the client side
 * would use this information to render the decoration.
 */
struct DecorationDef {
  /**
   * Name of the decoration.
   */
  std::string name;
};

} // namespace freeisle::def
