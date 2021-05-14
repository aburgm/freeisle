#pragma once

#include "defs/DecorationDef.hh"
#include "defs/TerrainType.hh"

#include "core/Grid.hh"

#include <string>

namespace freeisle::def {

/**
 * Definition of the map.
 */
struct MapDef {
  /**
   * Definition of a single hex tile.
   */
  struct Hex {
    /**
     * The base terrain type of this hex, e.g. grass, snow, etc.
     */
    TerrainType baseTerrain;

    /**
     * The overlay terrain type of this hex. This is a terrain that
     * sits on top of the base terrain, e.g. street on top of grass,
     * forest on top of snow, street on top of water (bridge), etc.
     */
    TerrainType overlayTerrain;

    /**
     * Decoration on this hex tile, if any.
     */
    const DecorationDef *decoration;
  };

  /**
   * Loaded decorations.
   */
  std::vector<DecorationDef> decorationDefs;

  /**
   * The main map grid.
   */
  Grid<Hex> grid;
};

} // namespace freeisle::def
