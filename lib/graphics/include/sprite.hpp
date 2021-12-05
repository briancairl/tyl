/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite.hpp
 */
#pragma once

// Tyl
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/fwd.hpp>

namespace tyl::graphics
{

struct SpriteBatchRenderProperties
{
  std::size_t max_sprite_count;
};

struct SpriteTileID
{
  std::size_t id;
};

ecs::entity create_sprite_batch_renderer(ecs::registry& registry, const std::size_t max_sprite_count);

void attach_sprite_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_sprite_count);

void render_sprites(ecs::registry& registry, const Vec2i& viewport_size);

ecs::entity create_sprite(
  ecs::registry& registry,
  ecs::Ref<Texture> atlas_texture,
  ecs::Ref<TileUVLookup> uv_lookup,
  const Position2D& sprite_position,
  const RectSize2D& sprite_size);

void attach_sprite(
  ecs::registry& registry,
  const ecs::entity entity_id,
  ecs::Ref<Texture> atlas_texture,
  ecs::Ref<TileUVLookup> uv_lookup,
  const Position2D& sprite_position,
  const RectSize2D& sprite_size);

}  // namespace tyl::graphics
