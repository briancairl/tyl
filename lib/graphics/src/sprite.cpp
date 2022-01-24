/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

void update_sprites(ecs::registry& registry, const duration dt)
{
  // Update looped dynamic sprite sequences
  registry.view<RenderingEnabled, SpriteSequenceLooped, SpriteSequence, SpriteTileID, duration>().each(
    [dt](SpriteSequence& sequence, SpriteTileID& tile, const duration& update_period) {
      sequence.period_since_last_update += dt;
      if (sequence.period_since_last_update < update_period)
      {
        return;
      }
      else if (tile.id == sequence.stop_id)
      {
        tile.id = sequence.start_id;
      }
      else
      {
        ++tile.id;
      }
      sequence.period_since_last_update = duration::zero();
    });

  // Update one-shot dynamic sprite sequences
  registry.view<RenderingEnabled, SpriteSequenceOneShot, SpriteSequence, SpriteTileID, duration>().each(
    [dt](SpriteSequence& sequence, SpriteTileID& tile, const duration& update_period) {
      sequence.period_since_last_update += dt;
      if (sequence.period_since_last_update > update_period and tile.id < sequence.stop_id)
      {
        ++tile.id;
        sequence.period_since_last_update = duration::zero();
      }
    });
}

ecs::entity create_sprite(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect,
  const std::size_t z_order)
{
  const ecs::entity entity_id = registry.create();
  attach_sprite(registry, entity_id, uv_lookup, sprite_rect, z_order);
  return entity_id;
}

void attach_sprite(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect,
  const std::size_t z_order)
{
  TYL_ASSERT_FALSE(registry.has<SpriteTileID>(entity_id));
  registry.emplace<ecs::Ref<Texture>>(entity_id, ecs::ref<ecs::Ref<Texture>>(uv_lookup));
  registry.emplace<ecs::Ref<TileUVLookup>>(entity_id, ecs::ref<TileUVLookup>(uv_lookup));
  registry.emplace<Rect2D>(entity_id, sprite_rect);
  registry.emplace<SpriteTileID>(entity_id, 0UL);
  registry.emplace<RenderOrder2D>(entity_id, z_order, sprite_rect.min().y());
}

void attach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id, const float rate, const bool looped)
{
  TYL_ASSERT_FALSE(registry.has<SpriteSequence>(entity_id));
  TYL_ASSERT_TRUE((registry.has<SpriteTileID, ecs::Ref<TileUVLookup>>(entity_id)));
  TYL_ASSERT_GT(rate, 0.0f);

  {
    auto [tile, tile_uv] = registry.get<SpriteTileID, ecs::Ref<TileUVLookup>>(entity_id);
    tile.id = 0;
    registry.emplace<SpriteSequence>(entity_id, tile.id, (*tile_uv).tile_count() - 1, duration::zero());
  }

  registry.emplace<duration>(entity_id, make_duration(1.f / rate));

  if (looped)
  {
    registry.emplace<SpriteSequenceLooped>(entity_id);
  }
  else
  {
    registry.emplace<SpriteSequenceOneShot>(entity_id);
  }
}

void detach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id)
{
  TYL_ASSERT_TRUE(registry.has<SpriteSequence>(entity_id));
  registry.remove<SpriteSequence, duration>(entity_id);
  registry.remove_if_exists<SpriteSequenceOneShot>(entity_id);
  registry.remove_if_exists<SpriteSequenceLooped>(entity_id);
}


void retrigger_sprite_sequence(const ecs::Ref<SpriteSequence, SpriteTileID> sprite)
{
  auto [sequence, tile] = sprite.get<SpriteSequence, SpriteTileID>();
  tile.id = sequence.start_id;
  sequence.period_since_last_update = duration::zero();
}

}  // namespace tyl::graphics
