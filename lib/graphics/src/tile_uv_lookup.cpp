/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_uv_lookup.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <cstdio>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

inline Vec2f rectified_uv_extents(const Size2i tile_size_px, const Texture& atlas_texture)
{
  // How many times each tile divides into the entire atlas evenly
  const Vec2i divs = atlas_texture.size().array() / tile_size_px.array();

  // Region of atlas filled by evenly divided tiles
  const Vec2i even = divs.array() * tile_size_px.array();

  return (even.array().cast<float>() / atlas_texture.size().array().cast<float>());
}

}  // namespace anonymous

TileUVLookup::TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture) :
    TileUVLookup{tile_size_px, atlas_texture, Rect2i{Vec2i{0, 0}, atlas_texture.size()}}
{}

TileUVLookup::TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture, const Rect2i& region) :
    TileUVLookup{tile_size_px, atlas_texture, {region}}
{}

TileUVLookup::TileUVLookup(
  const Size2i tile_size_px,
  const Texture& atlas_texture,
  std::initializer_list<Rect2i> regions) :
    ecs::make_ref_from_this<TileUVLookup>{},
    tile_size_px_{tile_size_px},
    tile_size_uv_{rectified_uv_extents(tile_size_px, atlas_texture).array() /
                  (atlas_texture.size().array() / tile_size_px.array()).cast<float>()}
{
  for (const auto& region : regions)
  {
    // Get the (N x M) tile count of the region
    const Vec2i region_tile_extents = region.size().array() / tile_size_px.array();

    // Get the origin of the region in UV space
    const Vec2f region_tile_origin = tile_size_uv_.array() * region.min().array().cast<float>();

    // Reserve buffer size for additional offsets
    tile_uv_offsets_.reserve(
      tile_uv_offsets_.size() + static_cast<std::size_t>(region_tile_extents.x() * region_tile_extents.y()));

    // Compute tile offset
    for (int x = 0; x < region_tile_extents.x(); ++x)
    {
      for (int y = 0; y < region_tile_extents.y(); ++y)
      {
        tile_uv_offsets_.emplace_back(
          region_tile_origin.x() + x * tile_size_uv_.x(), region_tile_origin.y() + y * tile_size_uv_.y());
      }
    }
  }
}

TileUVLookup::~TileUVLookup() = default;

}  // namespace tyl::graphics
