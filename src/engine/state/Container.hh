#pragma once

#include "def/ContainerDef.hh"

#include "core/Ptr.hh"

#include <list>

namespace freeisle::state {

struct Unit; // Need forward decl to break cyclic dependency since units can be
             // containers

/**
 * Container state contains information on which units are contained
 * in a container.
 */
struct Container {
  // not copyable due to references to other objects
  Container(const Container &) = delete;
  Container(Container &&) = delete;

  Container &operator=(const Container &) = delete;
  Container &operator=(Container &&) = delete;

  /**
   * Container definition specifying what is allowed to
   * be contained in this container.
   */
  const def::ContainerDef *def;

  /**
   * The units currently being contained.
   */
  std::list<core::Ptr<Unit>> units;
};

} // namespace freeisle::state
