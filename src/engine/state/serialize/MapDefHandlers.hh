#pragma once

#include "state/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

#include "def/MapDef.hh"

namespace freeisle::state::serialize {

class DecorationDefLoader {
public:
  DecorationDefLoader(std::map<uint32_t, uint32_t> &indices);

  void set(def::DecorationDef &def);
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::DecorationDef *def_;
  std::map<uint32_t, uint32_t> &indices;
};

class DecorationDefSaver {
public:
  DecorationDefSaver();

  void set(const def::DecorationDef &def);
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::DecorationDef *def_;
  uint32_t index_;
};

class DecorationDefContainerLoader {
public:
  DecorationDefContainerLoader(std::vector<def::DecorationDef> &container,
                               AuxData &aux);

  void load(json::loader::Context &ctx, Json::Value &value);

  const def::DecorationDef *get_decoration_for_index(uint32_t index) const;

private:
  std::vector<def::DecorationDef> &container;
  std::map<uint32_t, uint32_t> indices;
  json::loader::MappedContainerHandler<std::vector<def::DecorationDef>,
                                       DecorationDefLoader>
      loader;
};

class MapDefLoader {
public:
  MapDefLoader(def::MapDef &map, AuxData &aux);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::MapDef &map_;
  AuxData &aux_;
};

class MapDefSaver {
public:
  MapDefSaver(const def::MapDef &map, AuxData &aux);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::MapDef &map_;
  AuxData &aux_;
};

} // namespace freeisle::state::serialize
