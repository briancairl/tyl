/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tilemap.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/tilemap.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics
{

void attach_tilemap(ecs::registry& reg, const ecs::entity e, const Vec2f& origin, const TileMapProperties& properties)
{
  reg.emplace<TileMapProperties>(e, properties);

  {
    auto& map = reg.emplace<MatXi>(e, properties.rows, properties.cols);
    map.setZero();
  }

  {
    const Vec2f min_corner = origin;
    const Vec2f max_corner =
      origin + Vec2f{properties.rows * properties.tile_size, properties.cols * properties.tile_size};
    reg.emplace<Rect2f>(e, min_corner, max_corner);
  }
}

std::optional<Rect2f> get_tile_rect(const ecs::registry& reg, const Vec2f& query)
{
  std::optional<Rect2f> rect{std::nullopt};

  auto view = reg.view<TileMapProperties, Rect2f>();
  for (const auto e : view)
  {
    if (const auto& r = view.get<const Rect2f>(e); r.within(query))
    {
      const auto& prop = view.get<const TileMapProperties>(e);

      // Compute offset between query and grid origin
      const Vec2f offset{query - r.min()};

      // Quantize offset
      const Vec2i quantized{(offset / prop.tile_size).cast<int>()};

      // Resolve lower corner of tile from quanitized coordinate
      const Vec2f min_corner{quantized.x() * prop.tile_size, quantized.y() * prop.tile_size};

      // Resolve upper corner
      const Vec2f max_corner{min_corner.x() + prop.tile_size, min_corner.y() + prop.tile_size};

      // Prepare return value as {min, max} corners
      rect.emplace(min_corner, max_corner);
      break;
    }
  }

  return rect;
}

}  // namespace tyl::graphics
