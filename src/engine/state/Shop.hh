#pragma once

#include "def/ShopDef.hh"
#include "state/Container.hh"
#include "state/Player.hh"

#include "core/Ptr.hh"

namespace freeisle::state {

/**
 * Shop stores the state of a shope in the game.
 */
struct Shop {
  // not copyable due to references to other objects
  Shop(const Shop &) = delete;
  Shop(Shop &&) = delete;

  Shop &operator=(const Shop &) = delete;
  Shop &operator=(Shop &&) = delete;

  /**
   * Definition of this shop.
   */
  const def::ShopDef *def;

  /**
   * Player who owns the shop. The shop is generating income for this player,
   * and this player can produce units in the shop.
   */
  core::Ptr<Player> owner;

  /**
   * State on which units are contained in the shop.
   */
  Container container;
};

} // namespace freeisle::state
