#pragma once

#include "state/Container.hh"
#include "state/Player.hh"
#include "state/Shop.hh"

#include "def/Collection.hh"
#include "def/UnitDef.hh"

namespace freeisle::state {

/**
 * Unit represents the state of a unit in the game.
 */
struct Unit {
  /**
   * General statistics about this particular unit. These are not relevant for
   * the game, but might be interesting to look at.
   */
  struct Stats {
    uint32_t hits_dealt;
    uint32_t hits_taken;

    uint32_t damage_dealt;
    uint32_t damage_taken;

    uint32_t hexes_moved;
  };

  /**
   * Unit definition with all static unit information.
   */
  def::NullableRef<def::UnitDef> def;

  /**
   * Player who controls this unit. If not set, the unit is not actively
   * controlled.
   */
  def::NullableRef<Player> owner;

  /**
   * Current location of the unit on the map.
   */
  def::Location location;

  /**
   * Health of the unit. 100 is full health, and 0 means the unit is destroyed
   * and taken out of the game.
   */
  uint32_t health;

  /**
   * Level of the unit. Typically the same as def->level, except the unit
   * is soared.
   */
  def::Level level;

  /**
   * Movement points the unit has left during this turn.
   */
  uint32_t movement;

  /**
   * Remaining fuel of the unit.
   */
  uint32_t fuel;

  /**
   * Experience of the unit. Every participation in combat increases experience
   * by 1. High experience inrceases the damage of the unit slightly.
   */
  uint32_t experience;

  /**
   * Whether the unit has performed an action (attack or supply) during
   * this turn.
   */
  bool has_actioned;

  /**
   * Whether the unit has soared (changed level) during this turn.
   */
  bool has_soared;

  /**
   * Supplies that this unit has left to supply other units with.
   */
  def::Resupply supplies;

  /**
   * Ammo the weapon systems of this unit have left. One entry in the
   * collection for each weapon defined in the unit def's WeaponDefs.
   */
  def::RefMap<def::WeaponDef, uint32_t> ammo;

  /**
   * Container state for transporter units.
   */
  Container container;

  /**
   * Pointer to the unit that is transporting this unit, if any.
   */
  def::NullableRef<Unit> contained_in_unit;

  /**
   * Pointer to the shop that this unit is inside, if any.
   */
  def::NullableRef<Shop> contained_in_shop;

  /**
   * General statistics for this unit.
   */
  Stats stats;
};

} // namespace freeisle::state
