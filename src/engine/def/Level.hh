#pragma once

namespace freeisle::def {

/**
 * Defines possible levels for a unit to be on.
 */
enum class Level {
  /**
   * Under water, e.g. submarine.
   */
  UnderWater = 0x00,

  /**
   * Water, e.g. boats, ships.
   */
  Water = 0x01,

  /**
   * Land, e.g. tanks.
   */
  Land = 0x02,

  /**
   * Air, e.g. helicopters and aeroplanes.
   */
  Air = 0x03,

  /**
   * High air, e.g. reconnaisance aircraft.
   */
  HighAir = 0x04,
};

} // namespace freeisle::def
