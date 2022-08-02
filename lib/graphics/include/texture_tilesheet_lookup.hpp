/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tilesheet_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/common/allocator.hpp>
#include <tyl/common/rect.hpp>

namespace tyl::graphics
{

/**
 * @brief Divides a texture, uniformly, into tiles
 */
struct UniformlyDividedRegion
{
  Vec2i subdivisions;
  Vec2i inner_padding_px;
  Rect2i area_px;
};

/**
 * @brief Specifies a transposed UniformlyDividedRegion
 */
struct TransposedUniformlyDividedRegion
{
  UniformlyDividedRegion region;
};

/**
 * @brief Lookup from ID to UV offset in an altas texture
 */
class TextureTilesheetLookup
{
public:
  TextureTilesheetLookup() = default;

  TextureTilesheetLookup(TextureTilesheetLookup&& other) = default;

  TextureTilesheetLookup(const TextureTilesheetLookup& other) = default;

  TextureTilesheetLookup(const Vec2i atlas_texture_extents, const Rect2i& region_descriptor);

  TextureTilesheetLookup(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region_descriptor);

  TextureTilesheetLookup(const Vec2i atlas_texture_extents, const TransposedUniformlyDividedRegion& region_descriptor);

  ~TextureTilesheetLookup();

  TextureTilesheetLookup& operator=(TextureTilesheetLookup&&) = default;

  void update(const Vec2i atlas_texture_extents, const Rect2i& region_descriptor);

  void update(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region_descriptor);

  void update(const Vec2i atlas_texture_extents, const TransposedUniformlyDividedRegion& region_descriptor);

  void update(const Rect2f& bounds) { tile_uv_bounds_.emplace_back(bounds); }

  void clear() { tile_uv_bounds_.clear(); }

  /**
   * @brief Returns total number of tile bounds
   */
  std::size_t tile_count() const { return tile_uv_bounds_.size(); }

  /**
   * @brief Returns tile bounds
   */
  const Rect2f& operator[](const std::size_t index) const { return tile_uv_bounds_[index]; }

  auto begin() const { return tile_uv_bounds_.begin(); }

  auto end() const { return tile_uv_bounds_.end(); }

private:
  /// Bounds for tiles in texture UV coordinate space
  std::vector<Rect2f, ContainerAllocator<Rect2f>> tile_uv_bounds_;
};

}  // namespace tyl::graphics