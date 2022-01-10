/**
 * @copyright 2022-present Brian Cairl
 *
 * @file game.cpp
 */

// Tyl
#include <tyl/components.hpp>
#include <tyl/game/tiled_region.hpp>

namespace tyl::game
{

ecs::entity
create_tiled_region(ecs::registry& registry, const Vec2f& position, const Vec2f& tile_size, const Vec2i dimensions)
{
  const auto e = registry.create();
  attach_tiled_region(registry, e, position, tile_size, dimensions);
  return e;
}

void attach_tiled_region(
  ecs::registry& registry,
  const ecs::entity region_id,
  const Vec2f& position,
  const Vec2f& tile_size,
  const Vec2i dimensions)
{
  const Vec2f region_size{tile_size.array() * dimensions.array().cast<float>()};

  registry.emplace<Position2D>(region_id);
  registry.emplace<Rect2D>(region_id, position, position + region_size);
  registry.emplace<TilingDimensions>(region_id, dimensions.x(), dimensions.y());
}

}  // namespace tyl::game
