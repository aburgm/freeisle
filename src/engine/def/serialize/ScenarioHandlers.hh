#pragma once

#include "def/serialize/AuxData.hh"

#include "json/Loader.hh"
#include "json/Saver.hh"

#include "def/Scenario.hh"

namespace freeisle::def::serialize {

class ScenarioLoader {
public:
  ScenarioLoader(def::Scenario &scenario, AuxData &aux);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::Scenario *scenario_;
  AuxData &aux_;
};

class ScenarioSaver {
public:
  ScenarioSaver(const def::Scenario &scenario, AuxData &aux,
                const std::string &map_filename);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::Scenario *scenario_;
  AuxData &aux_;
  const std::string map_filename_;
};

} // namespace freeisle::def::serialize
