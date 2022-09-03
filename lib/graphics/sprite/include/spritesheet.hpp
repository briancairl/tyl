/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tilesheet_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/math/rect.hpp>

namespace tyl::graphics::sprite
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
class SpriteSheet
{
public:
  SpriteSheet() = default;

  SpriteSheet(SpriteSheet&& other) = default;

  SpriteSheet(const SpriteSheet& other) = default;

  SpriteSheet(const Vec2i atlas_texture_extents, const Rect2i& region_descriptor);

  SpriteSheet(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region_descriptor);

  SpriteSheet(const Vec2i atlas_texture_extents, const TransposedUniformlyDividedRegion& region_descriptor);

  explicit SpriteSheet(std::vector<Rect2f>&& tile_uv_bounds);

  ~SpriteSheet();

  SpriteSheet& operator=(SpriteSheet&&) = default;
  SpriteSheet& operator=(std::vector<Rect2f>&& tile_uv_bounds);

  void update(const Vec2i atlas_texture_extents, const Rect2i& region_descriptor);

  void update(const Vec2i atlas_texture_extents, const UniformlyDividedRegion& region_descriptor);

  void update(const Vec2i atlas_texture_extents, const TransposedUniformlyDividedRegion& region_descriptor);

  void update(const Rect2f& bounds) { tile_uv_bounds_.emplace_back(bounds); }

  void clear() { tile_uv_bounds_.clear(); }

  /**
   * @brief Returns all bounds info
   */
  [[nodiscard]] constexpr const auto& bounds() const { return tile_uv_bounds_; }

  /**
   * @brief Returns total number of tile bounds
   */
  [[nodiscard]] std::size_t size() const { return tile_uv_bounds_.size(); }

  /**
   * @brief Returns tile bounds
   */
  [[nodiscard]] const Rect2f& operator[](const std::size_t index) const { return tile_uv_bounds_[index]; }

  [[nodiscard]] auto begin() const { return tile_uv_bounds_.begin(); }

  [[nodiscard]] auto end() const { return tile_uv_bounds_.end(); }

private:
  /// Bounds for tiles in texture UV coordinate space
  std::vector<Rect2f> tile_uv_bounds_;
};

}  // namespace tyl::graphics::sprite