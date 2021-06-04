#pragma once

#include "core/Enum.hh"
#include "core/EnumMap.hh"

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

constexpr core::EnumEntry<DamageType> DamageTypes[] = {
    {DamageType::SmallCaliber, "small_caliber"},
    {DamageType::BigCaliber, "big_caliber"},
    {DamageType::Missile, "missile"},
    {DamageType::Energy, "energy"},
    {DamageType::Explosive, "explosive"},
};

constexpr core::EnumMap<const char *, DamageType> DamageTypeNames =
    core::get_enum_names(DamageTypes);

} // namespace freeisle::def
