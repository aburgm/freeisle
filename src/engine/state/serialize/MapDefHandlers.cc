#include "state/serialize/MapDefHandlers.hh"

#include "json/LoadUtil.hh"
#include "json/SaveUtil.hh"

#include "fs/Path.hh"
#include "log/Logger.hh"

#include "base64/Base64.hh"
#include "png/Png.hh"

namespace freeisle::state::serialize {

DecorationDefLoader::DecorationDefLoader(std::map<uint32_t, uint32_t> &indices)
    : def_(nullptr), indices(indices) {}

void DecorationDefLoader::set(def::DecorationDef &def) { def_ = &def; }

void DecorationDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  def_->name = json::loader::load<std::string>(ctx, value, "name");

  const uint32_t index = json::loader::load<uint32_t>(ctx, value, "index");
  if (indices.find(index) != indices.end()) {
    throw json::loader::Error::create(ctx, "index", value["index"],
                                      "Duplicate index");
  }

  if (index == 0) {
    throw json::loader::Error::create(ctx, "index", value["index"],
                                      "Index 0 is not allowed");
  }

  if (index > 0xff) {
    // it would not be adressable by Rgb8 encoding
    throw json::loader::Error::create(ctx, "index", value["index"],
                                      "Index 256 or greater not allowed");
  }

  indices[index] = indices.size();
}

DecorationDefSaver::DecorationDefSaver() : def_(nullptr), index_(0) {}

void DecorationDefSaver::set(const def::DecorationDef &def) { def_ = &def; }

void DecorationDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  json::saver::save(ctx, value, "name", def_->name);
  json::saver::save(ctx, value, "index", ++index_);

  if (index_ > 0xff) {
    throw std::runtime_error("Too many decoration defs");
  }
}

DecorationDefContainerLoader::DecorationDefContainerLoader(
    std::vector<def::DecorationDef> &container, AuxData &aux)
    : container(container), loader(&aux.object_ids_, indices) {
  loader.set(container);
}

void DecorationDefContainerLoader::load(json::loader::Context &ctx,
                                        Json::Value &value) {
  loader.load(ctx, value);
}

const def::DecorationDef *
DecorationDefContainerLoader::get_decoration_for_index(uint32_t index) const {
  std::map<uint32_t, uint32_t>::const_iterator iter = indices.find(index);
  if (iter == indices.end()) {
    return nullptr;
  }

  assert(iter->second < container.size());
  return &container[iter->second];
}

MapDefLoader::MapDefLoader(def::MapDef &map, AuxData &aux)
    : map_(map), aux_(aux) {}

void MapDefLoader::load(json::loader::Context &ctx, Json::Value &value) {
  aux_.logger.info("Loading map definition...");

  DecorationDefContainerLoader decorations_loader(map_.decoration_defs, aux_);
  json::loader::load_object(ctx, value, "decorations", decorations_loader);

  const std::string map = json::loader::load<std::string>(ctx, value, "grid");

  core::Grid<core::color::Rgb8> image_data;

  try {
    std::vector<uint8_t> png_data;
    if (ctx.current_source->path.empty()) {
      // Decode from base64:
      png_data.resize((map.length() * 3 + 3) / 4);
      uint64_t len =
          base64::decode(reinterpret_cast<const uint8_t *>(map.data()),
                         map.length(), png_data.data());
      png_data.resize(len);
    } else {
      // Load from file:
      const std::string resolved = fs::path::resolve(map);
      if (fs::path::is_absolute(resolved)) {
        throw json::loader::Error::create(ctx, "grid", value["grid"],
                                          "Map path must not be absolute");
      }

      const std::string full_path =
          fs::path::join(fs::path::dirname(ctx.current_source->path), resolved);
      png_data = fs::read_file(full_path.c_str(), nullptr);
    }

    image_data = png::decode_rgb8(png_data.data(), png_data.size(),
                                  aux_.logger.make_child_logger("png-decode"));
  } catch (const std::exception &ex) {
    throw json::loader::Error::create(ctx, "grid", value["grid"], ex.what());
  }

  map_.grid =
      core::Grid<def::MapDef::Hex>(image_data.width(), image_data.height());
  for (uint32_t y = 0; y < image_data.height(); ++y) {
    for (uint32_t x = 0; x < image_data.width(); ++x) {
      uint32_t base_terrain_index = image_data(x, y).r;
      uint32_t overlay_terrain_index = image_data(x, y).g;
      uint32_t decoration_index = image_data(x, y).b;

      if (base_terrain_index >=
          static_cast<uint32_t>(def::BaseTerrainType::Num)) {
        throw json::loader::Error::create(
            ctx, "grid", value["grid"],
            fmt::format(
                "Map at {},{}: invalid base terrain type {} in R channel", x, y,
                base_terrain_index));
      }

      core::Sentinel<def::OverlayTerrainType, def::OverlayTerrainType::Num>
          overlay_terrain;
      if (overlay_terrain_index >
          static_cast<uint32_t>(def::OverlayTerrainType::Num)) {
        throw json::loader::Error::create(
            ctx, "grid", value["grid"],
            fmt::format(
                "Map at {},{}: invalid overlay terrain type {} in G channel", x,
                y, overlay_terrain_index));
      }

      if (overlay_terrain_index > 0) {
        overlay_terrain =
            static_cast<def::OverlayTerrainType>(overlay_terrain_index - 1);
      }

      const def::DecorationDef *deco = nullptr;
      if (decoration_index != 0) {
        deco = decorations_loader.get_decoration_for_index(decoration_index);
        if (deco == nullptr) {
          throw json::loader::Error::create(
              ctx, "grid", value["grid"],
              fmt::format(
                  "Map at {},{}: invalid decoration index {} in B channel", x,
                  y, decoration_index));
        }
      }

      map_.grid(x, y) = def::MapDef::Hex{
          .base_terrain = static_cast<def::BaseTerrainType>(base_terrain_index),
          .overlay_terrain = overlay_terrain,
          .decoration = deco};
    }
  }
}

MapDefSaver::MapDefSaver(const def::MapDef &map, AuxData &aux)
    : map_(map), aux_(aux) {}

void MapDefSaver::save(json::saver::Context &ctx, Json::Value &value) {
  aux_.logger.info("Saving map definition...");

  json::saver::MappedContainerHandler<std::vector<def::DecorationDef>,
                                      DecorationDefSaver>
      decorations_saver;
  decorations_saver.set(map_.decoration_defs);
  json::saver::save_object(ctx, value, "decorations", decorations_saver);

  // Encode hex grid as RGB8 image data, so we can save it as PNG
  core::Grid<core::color::Rgb8> image_data(map_.grid.width(),
                                           map_.grid.height());
  for (uint32_t y = 0; y < map_.grid.height(); ++y) {
    for (uint32_t x = 0; x < map_.grid.width(); ++x) {
      image_data(x, y).r = static_cast<uint32_t>(map_.grid(x, y).base_terrain);

      if (!map_.grid(x, y).overlay_terrain) {
        image_data(x, y).g = 0;
      } else {
        image_data(x, y).g =
            static_cast<uint32_t>(*map_.grid(x, y).overlay_terrain) + 1;
      }

      if (map_.grid(x, y).decoration == nullptr) {
        image_data(x, y).b = 0;
      } else {
        image_data(x, y).b =
            map_.grid(x, y).decoration - &map_.decoration_defs[0] + 1;
      }
    }
  }

  std::vector<uint8_t> png_data =
      png::encode_rgb8(image_data, aux_.logger.make_child_logger("png-encode"));

  if (!ctx.path.empty()) {
    const std::string png_path =
        fs::path::join(fs::path::dirname(ctx.path), "map.png");
    fs::write_file(png_path.c_str(), png_data.data(), png_data.size(), nullptr);
    json::saver::save(
        ctx, value, "grid",
        fs::path::make_relative(png_path, fs::path::dirname(ctx.path)));
  } else {
    std::string base64_encoded;
    base64_encoded.resize(
        base64::round_to_next_multiple_of<4>(png_data.size() * 4 / 3));
    base64::encode(png_data.data(), png_data.size(),
                   reinterpret_cast<uint8_t *>(&base64_encoded[0]));
    json::saver::save(ctx, value, "grid", base64_encoded);
  }
}

} // namespace freeisle::state::serialize
