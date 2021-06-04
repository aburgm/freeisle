#pragma once

#include "def/DamageType.hh"

#include <string>

namespace freeisle::def {

/**
 * Weapon definition for units. A unit can have multiple weapons.
 */
struct WeaponDef {
  /**
   * The name of the weapon.
   */
  std::string name;

  /**
   * The type of damage that the weapon inflicts.
   */
  DamageType damage_type;

  /**
   * The base damage that the weapon inflicts; before modifiers due to
   * terrian, resistance/vulnerability or hamming are applied.
   */
  uint32_t damage;

  /**
   * Minimum range of the weapon; the weapon cannot hit targets that
   * are closer to the unit than this number of hex tiles.
   */
  uint32_t min_range;

  /**
   * Maximum range of the weapon; the weapon cannot hit targets that
   * are further away from the unit than this number of hex tiles.
   */
  uint32_t max_range;

  /**
   * Maximum ammo that the unit carries for this weapon. The weapon
   * cannot be operated when it is out of ammo.
   */
  uint32_t ammo;
};

} // namespace freeisle::def
