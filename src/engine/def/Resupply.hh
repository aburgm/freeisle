#pragma once

#include "def/DamageType.hh"

#include "core/EnumMap.hh"

namespace freeisle::def {

/**
 * Resupply definition. If any of the fields are nonzero, specifies that
 * a unit can be resupplied with that kind of resource.
 */
struct Resupply {
  /**
   * Fuel resupply.
   */
  uint32_t fuel;

  /**
   * Health resupply.
   */
  uint32_t repair;

  /**
   * Ammo resupply for the various damage types.
   */
  core::EnumMap<uint32_t, DamageType> ammo;
};

} // namespace freeisle::def
