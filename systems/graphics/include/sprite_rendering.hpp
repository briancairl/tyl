/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_rendering.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/ecs/reference.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/texture_management.hpp>

namespace tyl::graphics
{

struct RenderTarget2D;

using AtlasTextureReference = ecs::ref<device::Texture, device::TextureHandle>;

struct SpriteRendererOptions
{
  /// Total number of sprites that can be rendered
  std::size_t capacity = 100UL;

  /// Texture unit to bind atlas texture to
  int atlas_texture_unit = 0;
};

struct SpriteRendererState
{
  /// Total number of sprites currently being rendered on update
  std::size_t size;
};

/**
 * @brief Creates a system which renders static/animated 2D sprites
 */
ecs::entity
create_sprite_renderer(ecs::registry& reg, const TextureReference& texture, const SpriteRendererOptions& options);

/**
 * @brief Attaches components for a system which renders static/animated 2D sprites
 */
void attach_sprite_renderer(ecs::registry& reg, const ecs::entity e, const SpriteRendererOptions& options);

/**
 * @brief Updates 2D sprite renderers
 */
void update_sprite_renderers(ecs::registry& reg, const RenderTarget2D& target);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteRendererOptions</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteRendererOptions>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteRendererOptions& options)
  {
    ar& named{"capacity", options.capacity};
    ar& named{"atlas_texture_unit", options.atlas_texture_unit};
  }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteRendererState</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteRendererState>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteRendererState& properties)
  {
    ar& named{"size", properties.size};
  }
};

}  // namespace tyl::serialization
