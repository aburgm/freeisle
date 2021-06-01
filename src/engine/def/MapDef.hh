#pragma once

#include "def/DecorationDef.hh"
#include "def/TerrainType.hh"

#include "core/Grid.hh"
#include "core/Sentinel.hh"

#include <string>
#include <vector>

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
    BaseTerrainType base_terrain;

    /**
     * The overlay terrain type of this hex. This is a terrain that
     * sits on top of the base terrain, e.g. street on top of grass,
     * forest on top of snow, street on top of water (bridge), etc.
     */
    core::Sentinel<OverlayTerrainType, OverlayTerrainType::Num> overlay_terrain;

    /**
     * Decoration on this hex tile, if any.
     */
    const DecorationDef *decoration = nullptr;
  };

  /**
   * Loaded decorations.
   */
  std::vector<DecorationDef> decoration_defs;

  /**
   * The main map grid.
   */
  core::Grid<Hex> grid;
};

} // namespace freeisle::def
