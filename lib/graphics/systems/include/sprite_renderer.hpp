/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_renderer.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/ecs/reference.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics::systems
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
create_sprite_renderer(ecs::registry& reg, const AtlasTextureReference& texture, const SpriteRendererOptions& options);

/**
 * @brief Attaches components for a system which renders static/animated 2D sprites
 */
void attach_sprite_renderer(ecs::registry& reg, const ecs::entity e, const SpriteRendererOptions& options);

/**
 * @brief Updates 2D sprite renderers
 */
void update_sprite_renderers(ecs::registry& reg, const RenderTarget2D& target);

}  // namespace tyl::graphics::systems
