#pragma once

#include "def/Collection.hh"
#include "def/MapDef.hh"
#include "def/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

#include "state/Player.hh"
#include "state/Team.hh"
#include "state/Unit.hh"

#include "core/Color.hh"

namespace freeisle::state::serialize {

class ColorLoader {
public:
  ColorLoader(core::color::Rgb8 &color);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  core::color::Rgb8 &color_;
};

class ColorSaver {
public:
  ColorSaver(const core::color::Rgb8 &color);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const core::color::Rgb8 &color_;
};

class TeamLoader {
public:
  TeamLoader();

  void set(state::Team &team);
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  state::Team *team_;
};

class TeamSaver {
public:
  TeamSaver();

  void set(const state::Team &team);
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const state::Team *team_;
};

/**
 * Player loader needs unit list populated for loading. Before loading players,
 * load units with empty player list so that owners of units can be populated.
 */
class PlayerLoader {
public:
  PlayerLoader(const def::MapDef &map, def::Collection<Team> &teams,
               def::Collection<Unit> &units, def::serialize::AuxData &aux);

  void set(state::Player &player);
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  state::Player *player_;
  def::serialize::AuxData &aux_;
  const def::MapDef &map_;
  def::Collection<Team> &teams_;
  def::Collection<Unit> &units_;
};

class PlayerSaver {
public:
  PlayerSaver(const def::MapDef &map, const def::Collection<Team> &teams,
              const def::Collection<Unit> &units, def::serialize::AuxData &aux);

  void set(const state::Player &player);
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const state::Player *player_;
  def::serialize::AuxData &aux_;
  const def::MapDef &map_;
  const def::Collection<Team> &teams_;
  const def::Collection<Unit> &units_;
  uint32_t player_index_;
};

} // namespace freeisle::state::serialize
