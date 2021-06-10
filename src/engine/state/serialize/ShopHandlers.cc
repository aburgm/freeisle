#include "state/serialize/ShopHandlers.hh"

#include "def/serialize/CollectionLoaders.hh"
#include "def/serialize/CollectionSavers.hh"

namespace freeisle::state::serialize {

ShopLoader::ShopLoader(def::Collection<def::ShopDef> &shop_defs, Map &map,
                       def::Collection<Player> &players)
    : shop_defs_(shop_defs), map_(map), players_(players) {}

void ShopLoader::set(def::Ref<Shop> shop) { shop_ = shop; }

void ShopLoader::load(json::loader::Context &ctx, Json::Value &value) {
  assert(shop_);

  shop_->def =
      def::serialize::load_mandatory_ref(ctx, value, "def", shop_defs_);
  shop_->owner =
      def::serialize::load_nullable_ref(ctx, value, "owner", players_);
  shop_->container.def = &shop_->def->container;
  // Contained units are populated at unit load time

  // should have been checked when loading location
  const def::Location &location = shop_->def->location;
  assert(location.x <= map_.grid.width() && location.y <= map_.grid.height());

  if (map_.grid(location.x, location.y).shop ||
      map_.grid(location.x, location.y).surface_unit ||
      map_.grid(location.x, location.y).subsurface_unit) {
    const std::string message = fmt::format(
        "Location x={}, y={} is already occupied", location.x, location.y);
    throw json::loader::Error::create(ctx, "def", value["def"], message);
  }

  map_.grid(location.x, location.y).shop = shop_;
}

ShopSaver::ShopSaver(const def::Collection<def::ShopDef> &shop_defs,
                     const def::Collection<Player> &players)
    : shop_(nullptr), shop_defs_(shop_defs), players_(players) {}

void ShopSaver::set(def::Ref<const Shop> shop) { shop_ = &*shop; }

void ShopSaver::save(json::saver::Context &ctx, Json::Value &value) {
  assert(shop_ != nullptr);

  def::serialize::save_ref(ctx, value, "def", shop_->def, shop_defs_);
  def::serialize::save_ref(ctx, value, "owner", shop_->owner, players_);
  // Contained units are not saved, they are saved as part of units
}

} // namespace freeisle::state::serialize
