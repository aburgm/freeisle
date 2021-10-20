#include "json/LoadUtil.hh"

#include "base64/Base64.hh"
#include "fs/File.hh"
#include "fs/Path.hh"

namespace freeisle::json::loader {

std::vector<uint8_t> load_binary(Context &ctx, Json::Value &value,
                                 const char *key) {
  const std::string val = json::loader::load<std::string>(ctx, value, key);

  try {
    if (core::string::has_prefix(val, "base64:")) {
      std::string_view view(val);
      view.remove_prefix(7);

      // Decode from base64:
      std::vector<uint8_t> data((view.length() * 3 + 3) / 4);
      uint64_t len =
          base64::decode(reinterpret_cast<const uint8_t *>(view.data()),
                         view.length(), data.data());
      data.resize(len);
      return data;
    } else if (core::string::has_prefix(val, "file:")) {
      if (ctx.current_source->path.empty()) {
        throw json::loader::Error::create(
            ctx, key, value[key], "Can't load file without path in context");
      }

      std::string_view view(val);
      view.remove_prefix(5);

      // Load from file:
      const std::string resolved = fs::path::resolve(view);
      if (fs::path::is_absolute(resolved)) {
        throw json::loader::Error::create(ctx, key, value[key],
                                          "Path must not be absolute");
      }

      const std::string full_path =
          fs::path::join(fs::path::dirname(ctx.current_source->path), resolved);
      return fs::read_file(full_path.c_str(), nullptr);
    } else {
      throw json::loader::Error::create(
          ctx, key, value[key],
          fmt::format("Unexpected binary schema for \"{}\"", val));
    }
  } catch (const json::loader::Error &err) {
    throw err;
  } catch (const std::exception &ex) {
    throw json::loader::Error::create(ctx, key, value[key], ex.what());
  }
}

} // namespace freeisle::json::loader
