#pragma once

#include "def/serialize/AuxData.hh"
#include "def/serialize/CollectionLoaders.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

#include "def/MapDef.hh"

namespace freeisle::def::serialize {

class DecorationDefLoader {
public:
  DecorationDefLoader(std::map<uint32_t, const def::DecorationDef *> &indices);

  void set(def::DecorationDef &def);
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::DecorationDef *def_;
  std::map<uint32_t, const def::DecorationDef *> &indices;
};

class DecorationDefSaver {
public:
  DecorationDefSaver(
      std::map<const def::DecorationDef *, uint32_t> &reverse_index_map);

  void set(const def::DecorationDef &def);
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::DecorationDef *def_;
  uint32_t index_;
  std::map<const def::DecorationDef *, uint32_t> &reverse_index_map_;
};

class DecorationDefContainerLoader {
public:
  DecorationDefContainerLoader(def::Collection<def::DecorationDef> &container,
                               AuxData &aux);

  void load(json::loader::Context &ctx, Json::Value &value);

  const def::DecorationDef *get_decoration_for_index(uint32_t index) const;

private:
  def::Collection<def::DecorationDef> &container;
  std::map<uint32_t, const def::DecorationDef *> indices;
  CollectionLoader<def::DecorationDef, DecorationDefLoader> loader;
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
  MapDefSaver(const def::MapDef &map, AuxData &aux,
              const std::string &map_filename);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::MapDef &map_;
  AuxData &aux_;
  const std::string map_filename_;
};

} // namespace freeisle::def::serialize
