/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/sprite.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

Vec2f rectify_uv_extents(const Vec2i tile_size_px, const Vec2i atlas_texture_size_px)
{
  // How many times each tile divides into the entire atlas evenly
  const Vec2i divs = atlas_texture_size_px.array() / tile_size_px.array();

  // Region of atlas filled by evenly divided tiles
  const Vec2i even = divs.array() * tile_size_px.array();

  return (even.array().cast<float>() / atlas_texture_size_px.array().cast<float>());
}

void update_sprite_sheet_bounds(std::vector<Rect2f>& bounds, const SpriteSheetLookupOptions& options)
{
  // Total pixels over the whole region contributed by padding
  const Vec2i total_inner_padding_px{
    (options.inner_padding_px.x() == 0) ? 0 : options.inner_padding_px.x() * (options.subdivisions.x() - 1),
    (options.inner_padding_px.y() == 0) ? 0 : options.inner_padding_px.y() * (options.subdivisions.y() - 1)};

  // The size of a subdivided tile, minus padding
  const Vec2i tile_size_px{(options.area_px.extents() - total_inner_padding_px).array() / options.subdivisions.array()};
  TYL_ASSERT_GT(tile_size_px.x(), 0);
  TYL_ASSERT_GT(tile_size_px.y(), 0);

  // The size of a subdivided + inner padding
  const Vec2i block_size_px{tile_size_px + options.inner_padding_px};
  TYL_ASSERT_GT(block_size_px.x(), 0);
  TYL_ASSERT_GT(block_size_px.y(), 0);

  // Normalized UV extents, clipped to nearest even subdivision
  const Vec2f texture_extents_uv{rectify_uv_extents(block_size_px, options.atlas_texture_size_px)};
  TYL_ASSERT_GT(texture_extents_uv.x(), 0.0f);
  TYL_ASSERT_GT(texture_extents_uv.y(), 0.0f);

  // Total nearest even subdivisions
  const Vec2i texture_total_subdivisions{options.atlas_texture_size_px.array() / block_size_px.array()};
  TYL_ASSERT_GT(texture_total_subdivisions.x(), 0);
  TYL_ASSERT_GT(texture_total_subdivisions.y(), 0);

  // Subdivided block size, in UV space
  const Vec2f block_size_uv{texture_extents_uv.array() / texture_total_subdivisions.array().cast<float>()};
  TYL_ASSERT_GT(block_size_uv.x(), 0.0f);
  TYL_ASSERT_GT(block_size_uv.y(), 0.0f);

  // UV-to-px conversion
  const Vec2f px_to_uv = (block_size_uv.array() / block_size_px.array().cast<float>());

  // Inner-padded tile size, in UV space
  const Vec2f tile_size_uv{px_to_uv.array() * tile_size_px.array().cast<float>()};
  TYL_ASSERT_GT(tile_size_uv.x(), 0.0f);
  TYL_ASSERT_GT(tile_size_uv.y(), 0.0f);

  // Reserve buffer size for additional offsets
  bounds.reserve(bounds.size() + static_cast<std::size_t>(options.subdivisions.array().prod()));

  // Get the origin of the region in UV space
  const Vec2f region_tile_origin = px_to_uv.array() * options.area_px.min().array().cast<float>();

  // Compute tile offset
  if (options.transpose)
  {
    for (int x = (options.subdivisions.x() - 1); x >= 0; --x)
    {
      const float lower_corner_x = region_tile_origin.x() + x * block_size_uv.x();
      for (int y = (options.subdivisions.y() - 1); y >= 0; --y)
      {
        const float lower_corner_y = region_tile_origin.y() + y * block_size_uv.y();
        bounds.emplace_back(
          lower_corner_x, lower_corner_y, lower_corner_x + tile_size_uv.x(), lower_corner_y + tile_size_uv.y());
      }
    }
  }
  else
  {
    for (int x = 0; x < options.subdivisions.x(); ++x)
    {
      const float lower_corner_x = region_tile_origin.x() + x * block_size_uv.x();
      for (int y = 0; y < options.subdivisions.y(); ++y)
      {
        const float lower_corner_y = region_tile_origin.y() + y * block_size_uv.y();
        bounds.emplace_back(
          lower_corner_x, lower_corner_y, lower_corner_x + tile_size_uv.x(), lower_corner_y + tile_size_uv.y());
      }
    }
  }
}

}  // namespace anonymous

ecs::entity
create_sprite(ecs::registry& reg, const Vec2f& position, const Size2f& size, const SpriteSheetLookupOptions& options)
{
  const auto e = reg.create();
  attach_sprite(reg, e, position, size, options);
  return e;
}

void attach_sprite(
  ecs::registry& reg,
  const ecs::entity e,
  const Vec2f& position,
  const Size2f& size,
  const SpriteSheetLookupOptions& options)
{
  reg.emplace<SpritePosition>(e, position);
  reg.emplace<SpriteSize>(e, size);
  update_sprite_sheet_bounds(reg.emplace<SpriteSheetLookup>(e).uv_bounds, options);
}

void update_sprite(ecs::registry& reg, const ecs::entity e, const SpriteSheetLookupOptions& options)
{
  TYL_ASSERT_TRUE(reg.all_of<SpriteSheetLookup>(e))
  update_sprite_sheet_bounds(reg.get<SpriteSheetLookup>(e).uv_bounds, options);
}


void update_sprite(ecs::registry& reg, const ecs::entity e, const Rect2f& uv_bounds)
{
  TYL_ASSERT_TRUE(reg.all_of<SpriteSheetLookup>(e))
  reg.get<SpriteSheetLookup>(e).uv_bounds.emplace_back(uv_bounds);
}

}  // namespace tyl::graphics
