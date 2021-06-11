#pragma once

#include "def/Collection.hh"
#include "def/MapDef.hh"

#include "state/Unit.hh"

#include "core/Grid.hh"

namespace freeisle::state {

/**
 * State of the map.
 */
struct Map {
  /**
   * Information stored for each hex of the map.
   */
  struct Hex {
    /**
     * Which unit is on the surface (or in the air) of this hex, if any.
     */
    def::Ref<Unit> surfaceUnit;

    /**
     * Which unit is below the surface (under water) on this hex, if any.
     */
    def::Ref<Unit> subSurfaceUnit;

    /**
     * Which shop is on this hex, if any.
     */
    def::Ref<Shop> shop;
  };

  /**
   * Map definition.
   */
  const MapDef *def;

  /**
   * Grid with information for all hex tiles.
   */
  core::Grid<Hex> grid;
};

} // namespace freeisle::state
