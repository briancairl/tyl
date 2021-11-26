/**
 * @copyright 2021-present Brian Cairl
 *
 * @file uv_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <vector>

// Tyl
#include <tyl/matrix.hpp>

namespace tyl::graphics
{

/**
 * @brief Describes a lookup table between tile ID and tile atlas UV offsets
 */
class UVLookup
{
public:
  UVLookup(const Vec2i& tile_size, const Vec2i& tile_altas_size);

  inline const Vec2f& operator[](const std::size_t index) const { return uv_offsets_[index]; }

  inline std::size_t tile_count() const { return uv_offsets_.size(); }

  inline const Vec2f& tile_size_uv() const { return uv_tile_size_; }

private:
  /// Size of tile in UV (texture) space
  Vec2f uv_tile_size_;

  /// Tile offsets in UV space
  std::vector<Vec2f> uv_offsets_;
};

}  // namespace tyl::graphics
