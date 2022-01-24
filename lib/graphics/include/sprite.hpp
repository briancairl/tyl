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
  duration period_since_last_update;
};

struct SpriteSequenceLooped
{};

struct SpriteSequenceOneShot
{};

void update_sprites(ecs::registry& registry, const duration dt);

ecs::entity create_sprite(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect,
  const std::size_t z_order = 0UL);

void attach_sprite(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect,
  const std::size_t z_order = 0UL);

void attach_sprite_sequence(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const float rate,
  const bool looped = false);

void detach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id);

void retrigger_sprite_sequence(const ecs::Ref<SpriteSequence, SpriteTileID> sprite);

}  // namespace tyl::graphics
