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
#include <tyl/time.hpp>

namespace tyl::graphics
{

struct SpriteBatchRenderProperties
{
  // Maximum number of sprites to be rendered
  std::size_t max_sprite_count;
};

struct SpriteTileID
{
  // Current UV-tile lookup ID
  std::size_t id;
};

struct SpriteSequence
{
  /// First ID
  std::size_t start_id;

  /// Last ID in the sequence
  std::size_t stop_id;

  /// Last sprite ID was updated
  time_point update_stamp;
};

struct SpriteSequenceLooped
{};

struct SpriteSequenceOneShot
{};

ecs::entity create_sprite_batch_renderer(ecs::registry& registry, const std::size_t max_sprite_count);

void attach_sprite_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_sprite_count);

void attach_sprite_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_sprite_count);

void render_sprites(ecs::registry& registry, const Target& render_target, const time_point stamp);

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

void attach_sprite_sequence(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t stop_id,
  const float rate,
  const bool looped = false);

void detach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id);

void retrigger_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id);

}  // namespace tyl::graphics
