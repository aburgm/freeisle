#pragma once

#include "state/serialize/AuxData.hh"

#include "json/Loader.hh"
#include "json/Saver.hh"

#include "def/Location.hh"
#include "def/MapDef.hh"
#include "def/ShopDef.hh"

namespace freeisle::state::serialize {

class LocationLoader {
public:
  LocationLoader(def::Location &loc, const def::MapDef &map);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::Location &loc_;
  const def::MapDef &map_;
};

class LocationSaver {
public:
  LocationSaver(const def::Location &loc);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::Location &loc_;
};

class ShopDefLoader {
public:
  ShopDefLoader(const def::MapDef &map,
                def::Collection<def::UnitDef> &unit_defs, AuxData &aux);

  void set(def::ShopDef &shop);
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::ShopDef *shop_;
  const def::MapDef &map_;
  def::Collection<def::UnitDef> &unit_defs_;
  AuxData &aux_;
};

class ShopDefSaver {
public:
  ShopDefSaver(const def::Collection<def::UnitDef> &unit_defs, AuxData &aux);

  void set(const def::ShopDef &shop);
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::ShopDef *shop_;
  const def::Collection<def::UnitDef> &unit_defs_;
  AuxData &aux_;
};

} // namespace freeisle::state::serialize
