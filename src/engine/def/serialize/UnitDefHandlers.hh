#pragma once

#include "def/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

#include "def/UnitDef.hh"

namespace freeisle::def::serialize {

class ResupplyLoader {
public:
  ResupplyLoader(def::Resupply &resupply) : resupply_(resupply) {}

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::Resupply &resupply_;
};

class ResupplySaver {
public:
  ResupplySaver(const def::Resupply &resupply) : resupply_(resupply) {}

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::Resupply &resupply_;
};

class ContainerDefLoader {
public:
  ContainerDefLoader(def::ContainerDef &container) : container_(container) {}

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::ContainerDef &container_;
};

class ContainerDefSaver {
public:
  ContainerDefSaver(const def::ContainerDef &container)
      : container_(container) {}

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::ContainerDef &container_;
};

class WeaponDefLoader {
public:
  WeaponDefLoader() : weapon_(nullptr) {}

  void set(def::WeaponDef &def) { weapon_ = &def; }
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::WeaponDef *weapon_;
};

class WeaponDefSaver {
public:
  WeaponDefSaver() : weapon_(nullptr) {}

  void set(const def::WeaponDef &def) { weapon_ = &def; }
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const def::WeaponDef *weapon_;
};

class UnitDefLoader {
public:
  UnitDefLoader(AuxData &aux) : aux_(aux), unit_(nullptr) {}

  void set(def::UnitDef &unit) { unit_ = &unit; }
  void load(json::loader::Context &ctx, Json::Value &value);

private:
  AuxData &aux_;
  def::UnitDef *unit_;
};

class UnitDefSaver {
public:
  UnitDefSaver(AuxData &aux) : aux_(aux), unit_(nullptr) {}

  void set(const def::UnitDef &unit) { unit_ = &unit; }
  void save(json::saver::Context &ctx, Json::Value &value);

private:
  AuxData &aux_;
  const def::UnitDef *unit_;
};

} // namespace freeisle::def::serialize
