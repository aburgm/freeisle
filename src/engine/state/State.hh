#pragma once

namespace freeisle::state {

/**
 * Represents the entire game state.
 */
struct State {
  // TODO(armin): implement deep copy for AI
  State(const State &) = delete;
  State(State &&) = delete;

  State &operator=(const State &) = delete;
  State &operator=(State &&) = delete;

  /**
   * Scenario that is being played.
   */
  const Scenario *scenario;

  /**
   * Teams participating.
   */
  std::vector<Team> teams;

  /**
   * Players participating, including eliminated players.
   */
  std::vector<Player> players;

  /**
   * State of all shops in the game.
   */
  std::vector<Shop> shops;

  /**
   * State of all units in the game.
   */
  std::list<Unit> units;

  /**
   * Current turn number.
   */
  uint32_t turn_num;

  /**
   * Player whose turn it currently is.
   */
  Ptr<Player> player_at_turn;
};

} // namespace freeisle::state
