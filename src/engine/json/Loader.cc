#include "json/Loader.hh"

#include "core/String.hh"
#include "fs/File.hh"
#include "fs/Path.hh"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>

namespace freeisle::json::loader {

namespace {

void merge_map(Json::Value &base, Json::Value &overlay,
               const SourceInfo &overlay_source, Context &ctx) {
  assert(base.isObject());
  assert(overlay.isObject());

  // resolve all includes before merging:
  {
    const TreeSourceChange sc(ctx, overlay_source);
    resolve_includes(ctx, overlay);
  }
  resolve_includes(ctx, base);

  const Json::Value::Members overlayMembers = overlay.getMemberNames();
  assert(std::find(overlayMembers.begin(), overlayMembers.end(), "include") ==
         overlayMembers.end());

  IncludeInfo &include_info = ctx.include_map[ctx.current_location];

  for (const std::string &key : overlayMembers) {
    Json::Value &value = overlay[key];

    if (base.isMember(key) && base[key].isObject() && value.isObject()) {
      const TreeLocationChange lc(ctx, key);
      merge_map(base[key], value, overlay_source, ctx);
    } else if (base.isMember(key)) {
      if (base[key].isNull()) {
        base.removeMember(key);
        // TODO(armin): some remark in IncludeInfo about this?
        // so that it can be replicated on save.
      }
    } else {
      base[key] = std::move(value);
      include_info.included_from[key] = &overlay_source;
    }
  }
}

std::pair<uint32_t, uint32_t> get_line_info(const uint8_t *data, size_t len,
                                            size_t offset) {
  assert(offset < len);
  uint32_t line = 0;
  uint32_t col = 0;
  for (const uint8_t *cur = data; cur != data + offset; ++cur) {
    if (*cur == '\n') {
      ++line;
      col = 0;
    } else {
      ++col;
    }
  }

  return {line + 1, col + 1};
}

const SourceInfo &get_source_for_key(const Context &ctx,
                                     const std::string &key) {
  std::map<std::string, IncludeInfo>::const_iterator iter =
      ctx.include_map.find(ctx.current_location);
  if (iter == ctx.include_map.end()) {
    return *ctx.current_source;
  }

  const IncludeInfo &include_info = iter->second;

  std::map<std::string, const SourceInfo *>::const_iterator include_iter =
      include_info.included_from.find(key);
  if (include_iter == include_info.included_from.end()) {
    return *ctx.current_source;
  }

  return *include_iter->second;
}

Json::Value read_json_document(const std::vector<uint8_t> &data) {
  Json::Reader reader;
  const char *begin = reinterpret_cast<const char *>(&data.data()[0]);
  const char *end = begin + data.size();

  Json::Value root;
  if (!reader.parse(begin, end, root, false)) {
    throw std::runtime_error(reader.getFormattedErrorMessages());
  }

  return root;
}

} // namespace

Error::Error(std::string formatted_message, std::string message,
             std::string path, uint32_t line, uint32_t col)
    : std::runtime_error(formatted_message), message_(std::move(message)),
      path_(std::move(path)), line_(line), col_(col) {}

Error Error::create(const Context &ctx, const std::string &key,
                    const Json::Value &val, std::string message) {
  const SourceInfo &info = get_source_for_key(ctx, key);
  const std::pair<uint32_t, uint32_t> line_info = get_line_info(
      info.source_data.data(), info.source_data.size(), val.getOffsetStart());

  std::string formatted_message;
  if (info.path.empty()) {
    formatted_message = fmt::format("line {}: {}", line_info.first, message);
  } else {
    formatted_message = fmt::format("file {}, line {}: {}", info.path,
                                    line_info.first, message);
  }

  return Error(formatted_message, std::move(message), info.path,
               line_info.first, line_info.second);
}

const std::string &Error::message() const { return message_; }

const std::string &Error::path() const { return path_; }

uint32_t Error::line() const { return line_; }

uint32_t Error::col() const { return col_; }

TreeSourceChange::TreeSourceChange(Context &ctx, const SourceInfo &source)
    : ctx_(ctx), source_(*ctx.current_source) {
  // TODO(armin): don't think this always holds, but ctx.current_source should
  // be somewhere within source's origin chain:
  assert(&source == ctx.current_source || source.origin == ctx.current_source);

  ctx.current_source = &source;
}

TreeSourceChange::~TreeSourceChange() { ctx_.current_source = &source_; }

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

TreeDescent::TreeDescent(Context &ctx, const std::string &key)
    : source_(ctx, get_source_for_key(ctx, key)), location_(ctx, key) {}

void resolve_includes(Context &ctx, Json::Value &value) {
  if (!value.isMember("include")) {
    return;
  }

  std::string filename = value["include"].asString();
  if (fs::path::is_absolute(filename)) {
    throw Error::create(ctx, "include", value["include"],
                        "Include path cannot be absolute");
  }

  try {
    filename = fs::path::resolve(filename);
  } catch (const std::invalid_argument &ex) {
    throw Error::create(ctx, "include", value["include"], ex.what());
  }

  // TODO(armin): before going through search paths, try from
  // dirname(info->path)?

  fs::File include_file;
  std::vector<std::string> tried_candidates;
  std::string full_path;
  uint32_t level;

  for (uint32_t i = ctx.current_source->level; i < ctx.search_paths.size();
       ++i) {
    std::string candidate_path = fs::path::join(ctx.search_paths[i], filename);

    try {
      include_file =
          fs::File(candidate_path.c_str(), fs::File::OpenMode::Read, nullptr);
      full_path = std::move(candidate_path);
      level = i;
      break;
    } catch (const std::exception &ex) {
      tried_candidates.push_back(candidate_path);
    }
  }

  if (!include_file) {
    if (tried_candidates.empty()) {
      throw Error::create(ctx, "include", value["include"],
                          "No search paths available for include resolution");
    }

    throw Error::create(
        ctx, "include", value["include"],
        fmt::format("Failed to find \"{}\"; tried: \"{}\"", filename,
                    core::string::join(tried_candidates.begin(),
                                       tried_candidates.end(), "\", \"")));
  }

  const fs::FileInfo file_info = include_file.info();
  std::vector<uint8_t> source_data(file_info.size);
  fs::read_all(include_file, source_data.data(), source_data.size());

  SourceInfo source{
      .filename = filename,
      .path = full_path,
      .id = file_info.id,
      .level = level,
      .origin = ctx.current_source,
      .source_data = std::move(source_data),
  };

  for (const SourceInfo *cur = source.origin; cur != nullptr;
       cur = cur->origin) {
    if (cur->id == source.id) {
      // TODO(armin): show information on where the original include directive
      // was
      throw Error::create(ctx, "include", value["include"],
                          "Cyclic include path");
    }
  }

  Json::Value root;
  try {
    root = read_json_document(source.source_data);
  } catch (const std::runtime_error &ex) {
    throw Error::create(
        ctx, "include", value["include"],
        fmt::format("Failed to parse JSON document at \"{}\": {}", full_path,
                    ex.what()));
  }

  ctx.sources.push_back(std::move(source));

  value.removeMember("include");
  merge_map(value, root, ctx.sources.back(), ctx);
}

std::pair<Context, Json::Value>
make_root_source_context(std::vector<uint8_t> data) {
  SourceInfo source{
      .filename = "",
      .path = "",
      .id = fs::FileId{},
      .level = 0,
      .origin = nullptr,
      .source_data = std::move(data),
  };

  Context ctx{
      .search_paths = {}, // this automatically disables all include resolution
      .sources = {std::move(source)},
      .current_location = "",
  };

  ctx.current_source = &ctx.sources.back();

  Json::Value root = read_json_document(ctx.sources.back().source_data);
  return {std::move(ctx), root};
}

std::pair<Context, Json::Value> make_root_file_context(const char *path) {
  fs::File file(path, fs::File::OpenMode::Read, nullptr);

  std::vector<uint8_t> data(file.info().size);
  fs::read_all(file, data.data(), data.size());

  SourceInfo source{
      .filename = path,
      .path = path,
      .id = file.info().id,
      .level = 0,
      .origin = nullptr,
      .source_data = std::move(data),
  };

  Context ctx{
      .search_paths = {fs::path::dirname(path)},
      .sources = {std::move(source)},
      .current_location = "",
  };

  ctx.current_source = &ctx.sources.back();

  Json::Value root = read_json_document(ctx.sources.back().source_data);
  return {std::move(ctx), root};
};

} // namespace freeisle::json::loader
