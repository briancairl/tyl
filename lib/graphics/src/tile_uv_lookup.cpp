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

template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};

template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

template <typename TextureRegionIteratorT>
void add_uv_regions(
  TileUVLookup& uv_lookup,
  const Texture& texture,
  const TextureRegionIteratorT first,
  const TextureRegionIteratorT last)
{
  std::for_each(first, last, [&uv_lookup, &texture](const TextureRegion& varg) {
    std::visit(
      overloaded{[&uv_lookup, &texture](const Rect2i& rect) { uv_lookup.update(texture, rect); },
                 [&uv_lookup, &texture](const UniformlyDividedRegion& region) { uv_lookup.update(texture, region); }},
      varg);
  });
}

}  // namespace anonymous

Vec2f rectified_uv_extents(const Size2i tile_size_px, const Texture& atlas_texture)
{
  // How many times each tile divides into the entire atlas evenly
  const Vec2i divs = atlas_texture.size().array() / tile_size_px.array();

  // Region of atlas filled by evenly divided tiles
  const Vec2i even = divs.array() * tile_size_px.array();

  return (even.array().cast<float>() / atlas_texture.size().array().cast<float>());
}

TileUVLookup::TileUVLookup(const TileUVLookup& reference, std::initializer_list<std::size_t> subset_indices)
{
  tile_uv_offsets_.reserve(std::distance(subset_indices.begin(), subset_indices.end()));
  std::transform(
    subset_indices.begin(),
    subset_indices.end(),
    std::back_inserter(tile_uv_offsets_),
    [&reference](const std::size_t index) { return reference.tile_uv_offsets_[index]; });
}

TileUVLookup::TileUVLookup(const Texture& atlas_texture, const Rect2i& region)
{
  TileUVLookup::update(atlas_texture, region);
  TYL_ASSERT_FALSE(tile_uv_offsets_.empty());
}

TileUVLookup::TileUVLookup(const Texture& atlas_texture, const UniformlyDividedRegion& region)
{
  TileUVLookup::update(atlas_texture, region);
  TYL_ASSERT_FALSE(tile_uv_offsets_.empty());
}

void TileUVLookup::update(const Texture& atlas_texture, const Rect2i& region)
{
  const Vec2i tile_size_px{region.extents()};
  const Vec2f tile_size_uv{rectified_uv_extents(tile_size_px, atlas_texture).array() /
                           (atlas_texture.size().array() / tile_size_px.array()).cast<float>()};

  // Get the origin of the region in UV space
  const Vec2f region_tile_origin = tile_size_uv.array() * region.min().array().cast<float>();

  // Compute tile offset
  tile_uv_offsets_.emplace_back(region_tile_origin.x(), region_tile_origin.y(), tile_size_uv.x(), tile_size_uv.y());
}

void TileUVLookup::update(const Texture& atlas_texture, const UniformlyDividedRegion& region)
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
  const Vec2f texture_extents_uv{rectified_uv_extents(block_size_px, atlas_texture)};
  TYL_ASSERT_GT(texture_extents_uv.x(), 0.0f);
  TYL_ASSERT_GT(texture_extents_uv.y(), 0.0f);

  // Total nearest even subdivisions
  const Vec2i texture_total_subdivisions{atlas_texture.size().array() / block_size_px.array()};
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
  tile_uv_offsets_.reserve(tile_uv_offsets_.size() + static_cast<std::size_t>(region.subdivisions.array().prod()));

  // Get the origin of the region in UV space
  const Vec2f region_tile_origin = px_to_uv.array() * region.area_px.min().array().cast<float>();

  // Compute tile offset
  if (region.reversed)
  {
    for (int x = (region.subdivisions.x() - 1); x >= 0; --x)
    {
      const float lower_corner_x = region_tile_origin.x() + x * block_size_uv.x();
      for (int y = (region.subdivisions.y() - 1); y >= 0; --y)
      {
        const float lower_corner_y = region_tile_origin.y() + y * block_size_uv.y();
        tile_uv_offsets_.emplace_back(lower_corner_x, lower_corner_y, tile_size_uv.x(), tile_size_uv.y());
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
        tile_uv_offsets_.emplace_back(lower_corner_x, lower_corner_y, tile_size_uv.x(), tile_size_uv.y());
      }
    }
  }
}

TileUVLookup::~TileUVLookup() = default;


ecs::entity create_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::Ref<Texture> texture,
  const std::initializer_list<TextureRegion>& regions)
{
  const ecs::entity e = registry.create();
  attach_tile_uv_lookup(registry, e, texture, regions);
  return e;
}

void attach_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<Texture> texture,
  const std::initializer_list<TextureRegion>& regions)
{
  registry.emplace<ecs::Ref<Texture>>(entity_id, texture);
  add_uv_regions(registry.emplace<TileUVLookup>(entity_id), (*texture), regions.begin(), regions.end());
}

ecs::entity create_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> reference,
  const std::initializer_list<std::size_t> subset_indices)
{
  const ecs::entity e = registry.create();
  attach_tile_uv_lookup(registry, e, reference, subset_indices);
  return e;
}

void attach_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> reference,
  const std::initializer_list<std::size_t> subset_indices)
{
  registry.emplace<ecs::Ref<Texture>>(entity_id, ecs::ref<ecs::Ref<Texture>>(reference));
  registry.emplace<TileUVLookup>(entity_id, reference.get<TileUVLookup>(), subset_indices);
}

}  // namespace tyl::graphics
