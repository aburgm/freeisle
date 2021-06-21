#pragma once

#include "state/State.hh"

#include "def/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

namespace freeisle::state::serialize {

class StateLoader {
public:
  StateLoader(State &state, def::Scenario &scenario,
              def::serialize::AuxData &aux);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  State &state_;
  def::Scenario &scenario_;
  def::serialize::AuxData &aux_;
};

class StateSaver {
public:
  StateSaver(const State &state, def::serialize::AuxData &aux);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const State &state_;
  def::serialize::AuxData &aux_;
};

} // namespace freeisle::state::serialize
