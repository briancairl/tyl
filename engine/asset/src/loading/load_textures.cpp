/**
 * @copyright 2023-present Brian Cairl
 *
 * @file load_textures.hpp
 */
// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/engine/asset/load_type.hpp>
#include <tyl/engine/asset/loading.hpp>
#include <tyl/engine/asset/types.hpp>
#include <tyl/engine/common/resources.hpp>
#include <tyl/engine/ecs/types.hpp>
#include <tyl/expected.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::engine::asset
{

void LoadTextures(LoadStatus& status, Collection& collection, Resources& resources)
{
  LoadType<Texture, Image>(
    status,
    collection.registry,
    resources,
    [](const std::filesystem::path& path) -> expected<Image, Error> {
      if (auto image_or_error = Image::load(path); image_or_error.has_value())
      {
        return std::move(image_or_error).value();
      }
      return make_unexpected(Error::kFailedToLoad);
    },
    [](Registry& registry, EntityID id, Image&& image) { registry.emplace<Texture>(id, image.texture()); });
}

}  // namespac etyl::engine::asset