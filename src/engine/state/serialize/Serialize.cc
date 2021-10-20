#include "state/serialize/Serialize.hh"

#include "def/serialize/MapDefHandlers.hh"
#include "def/serialize/ScenarioHandlers.hh"
#include "def/serialize/UnitDefHandlers.hh"
#include "state/serialize/StateHandlers.hh"

#include "json/LoadUtil.hh"
#include "json/Loader.hh"

#include "fs/Path.hh"

namespace freeisle::state::serialize {

namespace {

class SerializableStateLoader {
public:
  SerializableStateLoader(SerializableState &state,
                          def::serialize::AuxData &aux)
      : state_(state), aux_(aux) {}

  void load(json::loader::Context &ctx, Json::Value &value) {
    state_.scenario = std::make_unique<def::Scenario>();
    def::serialize::ScenarioLoader scenarioLoader(*state_.scenario, aux_);
    json::loader::load_object(ctx, value, "scenario", scenarioLoader);

    serialize::StateLoader stateLoader(state_.state, *state_.scenario, aux_);
    stateLoader.load(ctx, value);
  }

private:
  SerializableState &state_;
  def::serialize::AuxData &aux_;
};

class SerializableStateSaver {
public:
  SerializableStateSaver(const State &state, def::serialize::AuxData &aux)
      : state_(state), aux_(aux) {}

  void save(json::saver::Context &ctx, Json::Value &value) {
    def::serialize::ScenarioSaver scenarioSaver(*state_.scenario, aux_,
                                                "map.png");
    json::saver::save_object(ctx, value, "scenario", scenarioSaver);

    serialize::StateSaver stateSaver(state_, aux_);
    stateSaver.save(ctx, value);
  }

private:
  const State &state_;
  def::serialize::AuxData &aux_;
};

} // namespace

SerializableState create_scenario(const CreateOptions &options,
                                  log::Logger logger) {
  if (options.players.empty()) {
    throw std::invalid_argument("Need at least one player");
  }

  log::Logger sub_logger = logger.make_child_logger("create_scenario");
  def::serialize::AuxData aux{.logger = sub_logger};

  std::unique_ptr<def::Scenario> scenario = std::make_unique<def::Scenario>(
      def::Scenario{.name = options.name,
                    .description = options.description,
                    .map = {.grid = core::Grid<def::MapDef::Hex>(
                                options.width, options.height)}});

  std::map<std::string, json::IncludeInfo> include_map;
  uint32_t num = 0;
  for (const std::string &str : options.unit_defs) {
    const std::string id = fmt::format("unitdef{:03}", ++num);
    std::pair<def::Collection<def::UnitDef>::iterator, bool> result =
        scenario->units.try_emplace(id);
    assert(result.second);

    def::serialize::UnitDefLoader loader(aux);
    loader.set(result.first);

    json::loader::load_root_object(
        fs::path::join(options.base_dir, str).c_str(), loader);
    include_map[fmt::format(".scenario.units.{}", id)].filename = str;
  }

  std::map<uint32_t, const def::DecorationDef *> indices;
  num = 0;
  for (const std::string &str : options.decoration_defs) {
    const std::string &id = fmt::format("deco{:03}", ++num);
    std::pair<def::Collection<def::DecorationDef>::iterator, bool> result =
        scenario->map.decoration_defs.try_emplace(id);
    assert(result.second);

    def::serialize::DecorationDefLoader loader(indices);
    loader.set(result.first);

    json::loader::load_root_object(
        fs::path::join(options.base_dir, str).c_str(), loader);
    include_map[fmt::format(".scenario.map.decorations.{}", id)].filename = str;
  }

  const def::Scenario *scenario_ptr = scenario.get();

  SerializableState state{
      .scenario = std::move(scenario),
      .state = {.scenario = scenario_ptr,
                .map =
                    {
                        .grid =
                            core::Grid<Map::Hex>(options.width, options.height),
                    },
                .turn_num = 1},
      .include_map = include_map,
  };

  num = 0;
  for (const CreateOptions::PlayerInfo &info : options.players) {
    const std::string &id = fmt::format("player{:03}", ++num);
    std::pair<def::Collection<state::Player>::iterator, bool> result =
        state.state.players.try_emplace(id);
    assert(result.second);

    result.first->second.name = info.name;
    result.first->second.color = info.color;
    result.first->second.fow =
        core::Grid<Player::Fow>(options.width, options.height);
  }

  state.state.player_at_turn = state.state.players.begin();
  return state;
}

SerializableState load(const char *path, log::Logger logger) {
  log::Logger sub_logger = logger.make_child_logger("load_state");
  def::serialize::AuxData aux{.logger = sub_logger};

  SerializableState result;
  SerializableStateLoader loader(result, aux);
  json::loader::load_root_object(path, loader);
  return result;
}

void save(const SerializableState &state, const char *path,
          log::Logger logger) {
  log::Logger sub_logger = logger.make_child_logger("save");
  def::serialize::AuxData aux{.logger = sub_logger};

  SerializableStateSaver saver(state.state, aux);
  json::saver::save_root_object(path, saver, &state.include_map);
}

void save(const State &state, const char *path, log::Logger logger) {
  log::Logger sub_logger = logger.make_child_logger("save");
  def::serialize::AuxData aux{.logger = sub_logger};

  // TODO(armin): also set map_filename to "", so that it embeds the map?
  SerializableStateSaver saver(state, aux);
  json::saver::save_root_object(path, saver, nullptr);
}

} // namespace freeisle::state::serialize
