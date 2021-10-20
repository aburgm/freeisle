#include "json/SaveUtil.hh"

#include "base64/Base64.hh"
#include "fs/File.hh"
#include "fs/Path.hh"

namespace freeisle::json::saver {

void save_binary(Context &ctx, Json::Value &value, const char *key,
                 const uint8_t *data, size_t len, const char *filename) {
  if (filename != NULL && filename[0] != '\0' && !ctx.path.empty()) {
    const std::string path =
        fs::path::join(fs::path::dirname(ctx.path), filename);
    fs::write_file(filename, data, len, nullptr);
    json::saver::save(ctx, value, key,
                      "file:" + std::string(fs::path::make_relative(
                                    path, fs::path::dirname(ctx.path))));
  } else {
    std::string base64_encoded;
    base64_encoded.resize(7 +
                          base64::round_to_next_multiple_of<4>(len * 4 / 3));
    memcpy(base64_encoded.data(), "base64:", 7);
    base64::encode(data, len, reinterpret_cast<uint8_t *>(&base64_encoded[7]));
    json::saver::save(ctx, value, key, base64_encoded);
  }
}

} // namespace freeisle::json::saver
