/**
 * @copyright 2021-present Brian Cairl
 *
 * @file uv_lookup.cpp
 */

// Tyl
#include <tyl/graphics/uv_lookup.hpp>

namespace tyl::graphics
{

UVLookup::UVLookup(const Vec2i& tile_size, const Vec2i& tile_altas_size)
{
  const int quantized_tile_rows = tile_altas_size.x() / tile_size.x();
  const int quantized_tile_cols = tile_altas_size.y() / tile_size.y();

  uv_tile_size_.x() = 1.f / quantized_tile_cols;
  uv_tile_size_.y() = 1.f / quantized_tile_rows;

  uv_offsets_.reserve(quantized_tile_rows * quantized_tile_cols);
  for (int x = 0; x < quantized_tile_rows; ++x)
  {
    for (int y = 0; y < quantized_tile_cols; ++y)
    {
      uv_offsets_.emplace_back(x * uv_tile_size_.x(), y * uv_tile_size_.y());
    }
  }
}

}  // namespace tyl::graphics
