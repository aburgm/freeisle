#pragma once

#include "state/Map.hh"
#include "state/Player.hh"
#include "state/Shop.hh"
#include "state/Unit.hh"

#include "def/Collection.hh"
#include "def/UnitDef.hh"
#include "def/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

namespace freeisle::state::serialize {

class UnitLoader {
public:
  /**
   * Units should have been empty-loaded already, so that contained-in-unit
   * references can be resolved.
   */
  UnitLoader(def::Collection<def::UnitDef> &unit_defs, Map &map,
             def::Collection<Shop> &shops, def::Collection<Unit> &units,
             def::Collection<Player> &players);

  void set(def::Ref<Unit> unit);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  void check_unit_container(json::loader::Context &ctx, def::Ref<Unit> unit,
                            Json::Value &value);

  void check_container(json::loader::Context &ctx, Json::Value &value,
                       const def::NullableRef<Shop> &parent_shop,
                       const def::NullableRef<Unit> &parent_unit,
                       const def::NullableRef<Player> &owner,
                       const def::Location &container_location,
                       const Container &container,
                       const def::ContainerDef &def);
  def::NullableRef<Unit> unit_;
  def::Collection<def::UnitDef> &unit_defs_;

  Map &map_;
  def::Collection<Shop> &shops_;
  def::Collection<Unit> &units_;
  def::Collection<Player> &players_;
};

class UnitSaver {
public:
  UnitSaver(const def::Collection<def::UnitDef> &unit_defs,
            const def::Collection<Shop> &shops,
            const def::Collection<Unit> &units,
            const def::Collection<Player> &players);

  void set(def::Ref<const Unit> unit);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const Unit *unit_;
  const def::Collection<def::UnitDef> &unit_defs_;
  const def::Collection<Shop> &shops_;
  const def::Collection<Unit> &units_;
  const def::Collection<Player> &players_;
};

} // namespace freeisle::state::serialize
