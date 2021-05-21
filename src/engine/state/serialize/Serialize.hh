#pragma once

#include "log/Logger.hh"

#include "def/Scenario.hh"
#include "state/State.hh"

#include "json/IncludeInfo.hh"

#include <string>
#include <vector>

namespace freeisle::state::serialize {

/**
 * Represents a loaded game state. This contains the state of the game,
 * together with some meta information about where it was loaded from so
 * that it can be saved back with the same references.
 */
struct SerializableState {
  /**
   * The loaded scenario.
   */
  std::unique_ptr<def::Scenario> scenario;

  /**
   * The loaded game state.
   */
  State state;

  /**
   * Include references.
   */
  std::map<std::string, json::IncludeInfo> include_map;
};

/**
 * Options for creating a new scenario.
 */
struct CreateOptions {
  struct PlayerInfo {
    std::string name;
    core::color::Rgb8 color;
  };

  /**
   * Name of the scenario.
   */
  std::string name;

  /**
   * Description of the scenario.
   */
  std::string description;

  /**
   * Width and height of the (initial) map.
   */
  uint32_t width, height;

  /**
   * (Initial) players on the map. Need at least one player.
   */
  std::vector<PlayerInfo> players;

  /**
   * Directory relative to which unit definitions and map
   * decorations are looked up.
   */
  std::string base_dir;

  /**
   * Files from which to load unit definitions.
   */
  const std::vector<std::string> unit_defs;

  /**
   * Files from which to load map decorations.
   */
  const std::vector<std::string> decoration_defs;
};

/**
 * Creates a new, empty scenario with corresponding state. Use state transforms
 * to modify further.
 */
SerializableState create_scenario(const CreateOptions &options,
                                  log::Logger logger);

/**
 * Load a game state from the given file.
 */
SerializableState load(const char *path, log::Logger logger);

/**
 * Store loaded game state. Definitions will be referenced where they
 * were loaded from.
 */
void save(const SerializableState &state, const char *path, log::Logger logger);

/**
 * Store loaded game state. Definitions will be packed into the output file
 * so that it is stand-alone.
 */
void save(const State &state, const char *path, log::Logger logger);

} // namespace freeisle::state::serialize
