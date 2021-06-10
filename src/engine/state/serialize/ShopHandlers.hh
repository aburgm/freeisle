#pragma once

#include "state/Map.hh"
#include "state/Player.hh"
#include "state/Shop.hh"

#include "def/Collection.hh"
#include "def/ShopDef.hh"
#include "def/serialize/AuxData.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

namespace freeisle::state::serialize {

class ShopLoader {
public:
  ShopLoader(def::Collection<def::ShopDef> &shop_defs, Map &map,
             def::Collection<Player> &players);

  void set(def::Ref<Shop> shop);

  void load(json::loader::Context &ctx, Json::Value &value);

private:
  def::NullableRef<Shop> shop_;
  def::Collection<def::ShopDef> &shop_defs_;
  Map &map_;
  def::Collection<Player> &players_;
};

class ShopSaver {
public:
  ShopSaver(const def::Collection<def::ShopDef> &shop_defs,
            const def::Collection<Player> &players);

  void set(def::Ref<const Shop> shop);

  void save(json::saver::Context &ctx, Json::Value &value);

private:
  const Shop *shop_;
  const def::Collection<def::ShopDef> &shop_defs_;
  const def::Collection<Player> &players_;
};

} // namespace freeisle::state::serialize
