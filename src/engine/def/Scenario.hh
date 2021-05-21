#pragma once

#include "def/MapDef.hh"
#include "def/ShopDef.hh"
#include "def/UnitDef.hh"

#include <string>

namespace freeisle::def {

/**
 * Scenario definition. A scenario contains the immutable parts of the game.
 * Once created, a scenario typically doesn't change and stays the same
 * throughout the game.
 *
 * Game state typically references resources that are part of the scenario,
 * such as the map, or unit definitions.
 *
 * Note that a scenario alone is typically not stored or loaded, but the
 * whole game state is. For example, the scenario does not contain
 * information about the (initial) placement of units: this is part of
 * the game state, because it is mutable; units can move and can be
 * destroyed.
 */
struct Scenario {
  /**
   * Human readable name or title of the scenario.
   */
  std::string name;

  /**
   * Description of the scenario.
   */
  std::string description;

  /**
   * The map of the scenario.
   */
  MapDef map;

  /**
   * The shops on the map.
   */
  std::vector<ShopDef> shops;

  /**
   * The units available in this scenario.
   */
  std::vector<UnitDef> units;
};

} // namespace freeisle::def
