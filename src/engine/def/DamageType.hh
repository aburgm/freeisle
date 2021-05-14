#pragma once

namespace freeisle::def {

/**
 * Defines different classes of damage that can be inflicted.
 */
enum class DamageType {
  /**
   * Small caliber gunshot, e.g. rifles, machine guns.
   */
  SmallCaliber,

  /**
   * Big caliber shots, e.g. tanks or artillery.
   */
  BigCaliber,

  /**
   * Damage from a (possibly ballistic) missile.
   */
  Missile,

  /**
   * Damage from an energy weapon, e.g. a laser.
   */
  Energy,

  /**
   * Explosive damage, e.g. from a bomb or mine.
   */
  Explosive,

  /**
   * Total number of different damage types.
   */
  Num,
};

} // namespace freeisle::def
