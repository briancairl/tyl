/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture_management.hpp
 */
#pragma once

// C++ Standard Library
#include <string>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/ecs/reference.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/math/vec.hpp>
#include <tyl/utility/alias.hpp>

namespace tyl::graphics
{

/// Weak reference to a texture resource
using TextureReference = ecs::ref<device::Texture, device::TextureHandle>;

/**
 * @brief Size of a texture
 */
using TextureSize = alias<Vec2i, decltype("TextureSize"_tag)>;

/**
 * @brief Asset data for a texture source image
 */
struct TextureSourceImageAssetData
{
  /// Path to the image
  std::string path;

  /// Flip image on load?
  bool flip_vertically;
};

/**
 * @brief Attaches texture and texture meta data components to an existing entity
 */
TextureReference attach_texture(ecs::registry& reg, const TextureSourceImageAssetData& asset_data);

/**
 * @brief Creates a new entity with a texture and texture meta data components
 */
TextureReference create_texture(ecs::registry& reg, const TextureSourceImageAssetData& asset_data);

/**
 * @brief Reloads all textures from disk
 */
void reload_textures(ecs::registry& reg);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>
#include <tyl/serial/math/vec.hpp>
#include <tyl/serial/utility/alias.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::TextureSourceImageAssetData</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::TextureSourceImageAssetData>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::TextureSourceImageAssetData& asset_data)
  {
    ar& named{"path", asset_data.path};
    ar& named{"flip_vertically", asset_data.flip_vertically};
  }
};

}  // namespace tyl::serialization
