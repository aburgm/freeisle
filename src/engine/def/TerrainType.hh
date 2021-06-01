#pragma once

namespace freeisle::def {

/**
 * Species the available terrain types.
 *
 * There is two groups of terrains: base terrains and overay terrains.
 * Base terrain is terrain that covers a whole hex tile, e.g. grass or snow.
 * Overlay terrain is a terrain type that sits on top of the base terrain,
 * such as road or forest.
 */
enum class BaseTerrainType {
  /**
   * Grass. Base terrain.
   */
  Grass,

  /**
   * Desert. Base terrain.
   */
  Desert,

  /**
   * Snow. Base terrain.
   */
  Snow,

  /**
   * Shallow water. Base terrain.
   */
  ShallowWater,

  /**
   * Deep water. Base terrain.
   */
  DeepWater,

  /**
   * Hills. Base terrain.
   */
  Hill,

  /**
   * Mountains. Base terrain.
   */
  Mountain,

  /**
   * Total number of base terrain types.
   */
  Num,
};

enum class OverlayTerrainType {
  /**
   * Forest. Overlay terrain.
   */
  Forest,

  /**
   * Road. Overlay terrain.
   */
  Road,

  /**
   * Crevice. Overlay terrain.
   */
  Crevice,

  /**
   * Man-made fortification. Overlay terrain.
   */
  Fortification,

  /**
   * Total number of overlay terrain types.
   */
  Num,
};

} // namespace freeisle::def
