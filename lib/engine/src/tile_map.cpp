/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_map.cpp
 */

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/engine/geometry.hpp>
#include <tyl/engine/tile_map.hpp>

namespace tyl::engine
{

Entity create_tile_map(
  ECSRegistry& registry,
  const Vec2i& dimensions,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id)
{
  const Entity new_entity = registry.create();
  add_tile_map(registry, new_entity, dimensions, tile_size, transform, initial_tile_id);
  return new_entity;
}

void add_tile_map(
  ECSRegistry& registry,
  const Entity entity,
  const Vec2i& dimensions,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id)
{
  registry.emplace<TileMapDimensions>(entity, dimensions);
  registry.emplace<TileMapGrid>(entity, dimensions.x(), dimensions.y()).fill(initial_tile_id);
  registry.emplace<TileSizePx>(entity, tile_size);
  registry.emplace<Transform>(entity, transform);
}

}  // namespace tyl::engine
