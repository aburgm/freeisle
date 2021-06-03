#include "json/LoadUtil.hh"

#include "base64/Base64.hh"
#include "fs/File.hh"
#include "fs/Path.hh"

namespace freeisle::json::loader {

std::vector<uint8_t> load_binary(Context &ctx, Json::Value &value,
                                 const char *key) {
  const std::string val = json::loader::load<std::string>(ctx, value, key);

  try {
    if (ctx.current_source->path.empty()) {
      // Decode from base64:
      std::vector<uint8_t> data((val.length() * 3 + 3) / 4);
      uint64_t len =
          base64::decode(reinterpret_cast<const uint8_t *>(val.data()),
                         val.length(), data.data());
      data.resize(len);
      return data;
    } else {
      // Load from file:
      const std::string resolved = fs::path::resolve(val);
      if (fs::path::is_absolute(resolved)) {
        throw json::loader::Error::create(ctx, key, value[key],
                                          "Path must not be absolute");
      }

      const std::string full_path =
          fs::path::join(fs::path::dirname(ctx.current_source->path), resolved);
      return fs::read_file(full_path.c_str(), nullptr);
    }
  } catch (const json::loader::Error &err) {
    throw err;
  } catch (const std::exception &ex) {
    throw json::loader::Error::create(ctx, key, value[key], ex.what());
  }
}

} // namespace freeisle::json::loader
