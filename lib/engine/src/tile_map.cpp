/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_map.cpp
 */

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/engine/geometry.hpp>
#include <tyl/engine/state.hpp>
#include <tyl/engine/tile_map.hpp>

namespace tyl::engine
{

Entity create_tile_map(
  ECSRegistry& registry,
  const TileMapDimensions& tile_map_dim,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id)
{
  const Entity new_entity = registry.create();
  add_tile_map(registry, new_entity, tile_map_dim, tile_size, transform, initial_tile_id);
  return new_entity;
}

void add_tile_map(
  ECSRegistry& registry,
  const Entity entity,
  const TileMapDimensions& tile_map_dim,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id)
{
  registry.emplace<TileMapDimensions>(entity, tile_map_dim);
  registry.emplace<TileMapGrid>(entity, tile_map_dim.x(), tile_map_dim.y()).fill(initial_tile_id);
  registry.emplace<TileSizePx>(entity, tile_size);
  registry.emplace<Transform>(entity, transform);
  registry.emplace<UpdateFlags>(entity, 0xFFFFFFFF);
}

}  // namespace tyl::engine
