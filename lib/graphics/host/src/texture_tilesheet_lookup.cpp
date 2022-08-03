/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture_tilesheet_lookup.cpp
 */

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/graphics/texture_tilesheet_lookup.hpp>

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

template <bool Reversed>
void compute_tilesheet_bounds(
  std::vector<Rect2f>& bounds,
  const Vec2i atlas_texture_extents,
  const UniformlyDividedRegion& region)
{
  // Total pixels over the whole region contributed by padding
  const Vec2i total_inner_padding_px{
    (region.inner_padding_px.x() == 0) ? 0 : region.inner_padding_px.x() * (region.subdivisions.x() - 1),
    (region.inner_padding_px.y() == 0) ? 0 : region.inner_padding_px.y() * (region.subdivisions.y() - 1)};

  // The size of a subdivided tile, minus padding
  const Vec2i tile_size_px{(region.area_px.extents() - total_inner_padding_px).array() / region.subdivisions.array()};
  TYL_ASSERT_GT(tile_size_px.x(), 0);
  TYL_ASSERT_GT(tile_size_px.y(), 0);

  // The size of a subdivided + inner padding
  const Vec2i block_size_px{tile_size_px + region.inner_padding_px};
  TYL_ASSERT_GT(block_size_px.x(), 0);
  TYL_ASSERT_GT(block_size_px.y(), 0);

  // Normalized UV extents, clipped to nearest even subdivision
  const Vec2f texture_extents_uv{rectify_uv_extents(block_size_px, atlas_texture_extents)};
  TYL_ASSERT_GT(texture_extents_uv.x(), 0.0f);
  TYL_ASSERT_GT(texture_extents_uv.y(), 0.0f);

  // Total nearest even subdivisions
  const Vec2i texture_total_subdivisions{atlas_texture_extents.array() / block_size_px.array()};
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
  bounds.reserve(bounds.size() + static_cast<std::size_t>(region.subdivisions.array().prod()));

  // Get the origin of the region in UV space
  const Vec2f region_tile_origin = px_to_uv.array() * region.area_px.min().array().cast<float>();

  // Compute tile offset
  if constexpr (Reversed)
  {
    for (int x = (region.subdivisions.x() - 1); x >= 0; --x)
    {
      const float lower_corner_x = region_tile_origin.x() + x * block_size_uv.x();
      for (int y = (region.subdivisions.y() - 1); y >= 0; --y)
      {
        const float lower_corner_y = region_tile_origin.y() + y * block_size_uv.y();
        bounds.emplace_back(lower_corner_x, lower_corner_y, tile_size_uv.x(), tile_size_uv.y());
      }
    }
  }
  else
  {
    for (int x = 0; x < region.subdivisions.x(); ++x)
    {
      const float lower_corner_x = region_tile_origin.x() + x * block_size_uv.x();
      for (int y = 0; y < region.subdivisions.y(); ++y)
      {
        const float lower_corner_y = region_tile_origin.y() + y * block_size_uv.y();
        bounds.emplace_back(lower_corner_x, lower_corner_y, tile_size_uv.x(), tile_size_uv.y());
      }
    }
  }
}

}  // namespace anonymous

TextureTilesheetLookup::TextureTilesheetLookup(const Vec2i atlas_texture_extents, const Rect2i& region)
{
  TextureTilesheetLookup::update(atlas_texture_extents, region);
  TYL_ASSERT_FALSE(tile_uv_bounds_.empty());
}

TextureTilesheetLookup::TextureTilesheetLookup(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region)
{
  TextureTilesheetLookup::update(atlas_texture_extents, region);
  TYL_ASSERT_FALSE(tile_uv_bounds_.empty());
}

TextureTilesheetLookup::TextureTilesheetLookup(
  const Vec2i atlas_texture_extents,
  const TransposedUniformlyDividedRegion& region)
{
  TextureTilesheetLookup::update(atlas_texture_extents, region);
  TYL_ASSERT_FALSE(tile_uv_bounds_.empty());
}

void TextureTilesheetLookup::update(const Vec2i atlas_texture_extents, const Rect2i& region_descriptor)
{
  const Vec2i tile_size_px{region_descriptor.extents()};
  const Vec2f tile_size_uv{
    rectify_uv_extents(tile_size_px, atlas_texture_extents).array() /
    (atlas_texture_extents.array() / tile_size_px.array()).cast<float>()};

  // Get the origin of the region in UV space
  const Vec2f region_tile_origin = tile_size_uv.array() * region_descriptor.min().array().cast<float>();

  // Compute tile offset
  tile_uv_bounds_.emplace_back(region_tile_origin.x(), region_tile_origin.y(), tile_size_uv.x(), tile_size_uv.y());
}

void TextureTilesheetLookup::update(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region_descriptor)
{
  compute_tilesheet_bounds<false>(tile_uv_bounds_, atlas_texture_extents, region_descriptor);
}

void TextureTilesheetLookup::update(
  const Vec2i atlas_texture_extents,
  const TransposedUniformlyDividedRegion& region_descriptor)
{
  compute_tilesheet_bounds<true>(tile_uv_bounds_, atlas_texture_extents, region_descriptor.region);
}

TextureTilesheetLookup::~TextureTilesheetLookup() = default;

}  // namespace tyl::graphics
