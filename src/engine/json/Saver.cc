#include "json/Saver.hh"

namespace freeisle::json::saver {

TreeLocationChange::TreeLocationChange(Context &ctx, const std::string &key)
    : ctx_(ctx), location_(ctx.current_location) {
  ctx.current_location = location_ + "." + key;
}

TreeLocationChange::~TreeLocationChange() {
  assert(ctx_.current_location.rfind('.') != std::string::npos);
  assert(ctx_.current_location == location_ ||
         ctx_.current_location.substr(0, ctx_.current_location.rfind('.')) ==
             location_);

  ctx_.current_location = location_;
}

void restore_includes(const Context &ctx, Json::Value &value) {
  const std::map<std::string, IncludeInfo>::const_iterator iter =
      ctx.include_map.find(ctx.current_location);

  if (iter != ctx.include_map.end()) {
    const Json::Value::Members members = value.getMemberNames();
    if (!iter->second.filename.empty()) {
      value["include"] = iter->second.filename;
    }

    // remove all members except the ones that were overridden
    for (const std::string &member : members) {
      const std::map<std::string, bool>::const_iterator override_iter =
          iter->second.override_keys.find(member);
      if (override_iter == iter->second.override_keys.end()) {
        value.removeMember(member);
      } else if (!override_iter->second) {
        value[member] = Json::Value::null;
      }
    }
  }
}

} // namespace freeisle::json::saver
