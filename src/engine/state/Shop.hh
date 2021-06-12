#pragma once

#include "def/Collection.hh"
#include "def/ShopDef.hh"
#include "state/Container.hh"
#include "state/Player.hh"

namespace freeisle::state {

/**
 * Shop stores the state of a shope in the game.
 */
struct Shop {
  /**
   * Definition of this shop. This is only nullable so that it can be
   * default-constructed, it is typically always expected to be set.
   */
  def::NullableRef<def::ShopDef> def;

  /**
   * Player who owns the shop. The shop is generating income for this player,
   * and this player can produce units in the shop. Can be null if nobody
   * owns the shop.
   */
  def::NullableRef<Player> owner;

  /**
   * State on which units are contained in the shop.
   */
  Container container;
};

} // namespace freeisle::state
