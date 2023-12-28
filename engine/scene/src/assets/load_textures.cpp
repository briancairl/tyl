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
#include <tyl/ecs.hpp>
#include <tyl/engine/assets/load.hpp>
#include <tyl/engine/shared_state.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::engine
{

using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

AssetLoadingStatus LoadTextures(Assets& assets, SharedState& shared_state)
{
  return Load<Texture, Image>(
    assets.registry,
    shared_state,
    [](const std::filesystem::path& path) -> expected<Image, AssetError> {
      if (auto image_or_error = Image::load(path); image_or_error.has_value())
      {
        return std::move(image_or_error).value();
      }
      return make_unexpected(AssetError::kFailedToLoad);
    },
    [](Registry& registry, EntityID id, Image&& image) { registry.emplace<Texture>(id, image.texture()); });
}

}  // namespace tyl::engine