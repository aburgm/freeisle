#pragma once

#include "def/ContainerDef.hh"
#include "def/Resupply.hh"
#include "def/WeaponDef.hh"

#include "core/Bitmask.hh"

#include <string>

namespace freeisle::def {

/**
 * Definition of a unit available in the game.
 */
struct UnitDef {
  /**
   * Specifies the possible capabilities of a unit.
   */
  enum class Cap {
    /**
     * This unit can capture shops.
     */
    Capture,

    /**
     * This unit can soar, i.e. change its level (e.g. from water to
     * underwater).
     */
    Soar,

    /**
     * This unit cannot attack or supply after movement.
     * Default: a unit can move and action in the same turn.
     */
    NoActionAfterMove,

    /**
     * This unit can move after performing an attack or supply action.
     * Default: A unit cannot move in the same turn after performanig an action.
     */
    MoveAfterAction,
  };

  /**
   * System-internal unique identifier of the unit. This is used for example
   * for frontends to know what graphics to render for the unit.
   */
  std::string id;

  /**
   * The human-readable name of the unit.
   */
  std::string name;

  /**
   * A human-readable description of the unit.
   */
  std::string description;

  /**
   * The (standard) level of the unit. If the unit has the soaring capability,
   * it can change the level.
   */
  Level level;

  /**
   * Capabilities that this unit has.
   */
  core::Bitmask<Cap> caps;

  /**
   * General armor class, i.e. protection against damage, of the unit.
   */
  uint32_t armor;

  /**
   * Movement points of the unit. In general, traversing one hex costs
   * 100 movement points, but there are many factors having an effect on
   * that.
   */
  uint32_t movement;

  /**
   * Available fuel of the unit. Traversing one hex costs one unit of fuel.
   * If there is no fuel left, the unit can no longer move.
   */
  uint32_t fuel;

  /**
   * Cost for the unit to move on a hex of each type. If both base and
   * overlay terrain specified, the overlay counts, except the unit's level
   * does not apply to the overlay terrain, e.g. a unit with water level on
   * a hex with base terrain water and overlay terrain road would apply
   * movement costs for water level.
   *
   * Default movement cost is 100, but can be lower to move faster over the
   * given terrain, or can be higher to move slower.
   */
  uint32_t movement_cost[static_cast<uint32_t>(BaseTerrainType::Num) +
                         static_cast<uint32_t>(OverlayTerrainType::Num)];

  /**
   * Extra protection of the unit on each terrain. Standard protection is 100.
   * A value less than that means a unit is more vulnerable on this terrain,
   * whereas a value higher than 100 means it is less vulnerable.
   *
   * If a hex has both base and overlay terrain, then also the overlay terrain
   * is taken, except when the unit's level doesn't apply to the overlay
   * terrain.
   */
  uint32_t protection[static_cast<uint32_t>(BaseTerrainType::Num) +
                      static_cast<uint32_t>(OverlayTerrainType::Num)];

  /**
   * Resistance and vulnerability of the unit to each damage type. Default
   * resistance is 100, with values less than than indicating a vulnerability
   * to the type of damage, and values greater than 100 indicate resistance.
   */
  uint32_t resistance[static_cast<uint32_t>(DamageType::Num)];

  /**
   * Supplies that this unit can supply other units with.
   */
  Resupply supplies;

  /**
   * Weapon specifications that this unit has.
   */
  std::vector<WeaponDef> weapons;

  /**
   * Container definition of this unit for transporters.
   */
  ContainerDef container;

  /**
   * Value of this unit. The player must pay this much in wealth in order to
   * produce the unit, and it also influences the repair price.
   */
  uint32_t value;

  /**
   * View range, in number of hex tiles. Removes fog of war in an area of this
   * range around the unit.
   */
  uint32_t view_range;

  /**
   * Jamming range of the vehicle, in number of hex tiles. Jams the view of
   * enemy units.
   */
  uint32_t jamming_range;
};

} // namespace freeisle::def
