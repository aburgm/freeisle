#pragma once

namespace freeisle::def {

/**
 * Specifies the different types of goals.
 */
enum class Goal {
  /**
   * A player is eliminated if its HQ has been captured by an opponent.
   */
  ConquerHq,

  /**
   * A player is eliminated when they have no more units left.
   */
  EliminatePlayer,

  /**
   * A player is eliminated when their captain unit is destroyed.
   */
  EliminateCaptain,
};

constexpr core::EnumEntry<Goal> Goals[] = {
    {Goal::ConquerHq, "conquer_hq"},
    {Goal::EliminatePlayer, "eliminate_player"},
    {Goal::EliminateCaptain, "eliminate_captain"},
};

} // namespace freeisle::def
