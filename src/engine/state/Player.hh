#pragma once

namespace freeisle::state {

/**
 * Represents a player taking part in the game.
 */
struct Player {
  // not copyable due to references to other objects
  Player(const Player &) = delete;
  Player(Player &&) = delete;

  Player &operator=(const Player &) = delete;
  Player &operator=(Player &&) = delete;

  /**
   * Fog of war information for a particular tile.
   */
  struct Fow {
    /**
     * Whether a hex is discovered or discovered.
     */
    bool discovered;

    /**
     * Current view factors. Own units in the vicinity increase the view
     * factor, while opposing units may decrease it (jamming).
     */
    int32_t view;
  };

  /**
   * Name of the player.
   */
  std::string name;

  /**
   * Color of the player.
   */
  Color color;

  /**
   * Team that this player is in, or null if they are not in any team.
   */
  Ptr<Team> team;

  /**
   * fog of war (fow) state for this player.
   */
  Grid<Fow> fow;

  /**
   * Current wealth of the player.
   */
  uint32_t wealth;

  /**
   * Unit designated as the captain for this player, or null if there is no
   * such unit. The unit must have the isCaptain flag set.
   */
  Ptr<Unit> captain;

  /**
   * List of all units of this player.
   */
  std::list<Ptr<Unit>> units;

  /**
   * Lose conditions for this player. If any of them is fulfilled, the player
   * loses the game immediately and is eliminated.
   */
  Bitmask<Goal> loseConditions;

  /**
   * Whether this player has been eliminated or is still actively playing.
   */
  bool isEliminated;
};

} // namespace freeisle::state
