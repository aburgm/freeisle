#pragma once

#include "state/Player.hh"
#include "state/Shop.hh"
#include "state/Team.hh"
#include "state/Unit.hh"

#include "def/Collection.hh"

namespace freeisle::state {

/**
 * Represents the entire game state.
 */
struct State {
  State() = default;

  // TODO(armin): implement deep copy for AI
  State(const State &) = delete;
  State(State &&) = default; // I think this one works out of the box; pointers
                             // will remain valid

  State &operator=(const State &) = delete;
  State &operator=(State &&) = delete;

  /**
   * Scenario that is being played.
   */
  const def::Scenario *scenario;

  /**
   * Teams participating.
   */
  def::Collection<Team> teams;

  /**
   * Players participating, including eliminated players.
   */
  def::Collection<Player> players;

  /**
   * State of all shops in the game.
   */
  def::Collection<Shop> shops;

  /**
   * State of all units in the game.
   */
  def::Collection<Unit> units;

  /**
   * Current turn number.
   */
  uint32_t turn_num;

  /**
   * Player whose turn it currently is.
   */
  def::Ref<Player> player_at_turn;
};

} // namespace freeisle::state
