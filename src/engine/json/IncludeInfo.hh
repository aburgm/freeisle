#pragma once

namespace freeisle::json {

/**
 * Information about include directives in a JSON document, so that
 * they can be replicated on save.
 */
struct IncludeInfo {
  /**
   * Relative path to include file. Empty if the include happened at
   * a higher level in the tree (and therefore should not be replicated
   * at this level).
   */
  std::string filename;

  /**
   * Keys that are defined in the source file and that override anything from
   * the include files. The boolean indicates whether it was overridden or
   * removed.
   */
  std::map<std::string, bool> override_keys;
};

} // namespace freeisle::json
