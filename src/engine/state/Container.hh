#pragma once

#include "def/Collection.hh"
#include "def/ContainerDef.hh"

#include <list>

namespace freeisle::state {

struct Unit; // Need forward decl to break cyclic dependency since units can be
             // containers

/**
 * Container state contains information on which units are contained
 * in a container.
 */
struct Container {
  /**
   * Container definition specifying what is allowed to
   * be contained in this container.
   */
  const def::ContainerDef *def;

  /**
   * The units currently being contained.
   *
   * (no-save)
   */
  std::list<def::Ref<Unit>> units;
};

} // namespace freeisle::state
