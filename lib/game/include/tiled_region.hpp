/**
 * @copyright 2022-present Brian Cairl
 *
 * @file actor.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/vec.hpp>

namespace tyl::game
{

struct TilingDimensions
{
  int rows;
  int cols;
};

ecs::entity
create_tiled_region(ecs::registry& registry, const Vec2f& position, const Vec2f& tile_size, const Vec2i dimensions);

void attach_tiled_region(
  ecs::registry& registry,
  const ecs::entity region_id,
  const Vec2f& position,
  const Vec2f& tile_size,
  const Vec2i dimensions);

}  // namespace tyl::game
