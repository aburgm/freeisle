#pragma once

#include "defs/Level.hh"

#include "core/Bitmask.hh"

namespace freeisle::def {

/**
 * Definition for a container that can contain units.
 */
struct ContainerDef {
  /**
   * Maximum number of units that can be contained. If 0,
   * the container cannot contain anything.
   */
  uint32_t maxUnits;

  /**
   * The maximum accumulated weight the container can carry. Even
   * if the unit limit is not reached yet, if the weight limit is
   * reached, no more units can be contained in this container.
   */
  uint32_t maxWeight;

  /**
   * Constraints the container to units at the specified level, e.g.
   * a container for land units only.
   */
  Bitmask<Level> supportedLevels;
};

} // namespace freeisle::def
