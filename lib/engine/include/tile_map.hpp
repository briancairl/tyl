/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_map.hpp
 */
#pragma once

// Tyl
#include <tyl/common/ecs.hpp>
#include <tyl/common/strong_alias.hpp>
#include <tyl/engine/fwd.hpp>

namespace tyl::engine
{

TYL_DEFINE_STRONG_ALIAS(TileMapGrid, MatXi);

TYL_DEFINE_STRONG_ALIAS(TileSizePx, Vec2i);

Entity create_tile_map(
  ECSRegistry& registry,
  const Vec2i& dimensions,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id = 0);

void add_tile_map(
  ECSRegistry& registry,
  const Entity entity,
  const Vec2i& dimensions,
  const TileSizePx& tile_size,
  const Transform& transform,
  const int initial_tile_id = 0);

}  // namespace tyl::engine
