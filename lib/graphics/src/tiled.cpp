/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tiled.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/tiled.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

ecs::entity create_tiled(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tile_rect)
{
  const ecs::entity entity_id = registry.create();
  attach_tiled(registry, entity_id, uv_lookup, tile_rect);
  return entity_id;
}

void attach_tiled(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tile_rect)
{
  TYL_ASSERT_FALSE(registry.has<TiledRegion>(entity_id));
  registry.emplace<ecs::Ref<Texture>>(entity_id, ecs::ref<ecs::Ref<Texture>>(uv_lookup));
  registry.emplace<ecs::Ref<TileUVLookup>>(entity_id, ecs::ref<TileUVLookup>(uv_lookup));
  registry.emplace<Rect2D>(entity_id, tile_rect);
  registry.emplace<TiledRegion>(entity_id).ids.fill(0);
  registry.emplace<RenderOrder2D>(entity_id, 0UL, tile_rect.min().y());
  registry.emplace<RenderingEnabled>(entity_id);
}

}  // namespace tyl::graphics
