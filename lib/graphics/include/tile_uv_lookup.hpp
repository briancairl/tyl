/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_uv_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <initializer_list>
#include <vector>

// Tyl
#include <tyl/graphics/fwd.hpp>
#include <tyl/rect.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Lookup from ID to UV offset in an altas texture
 */
class TileUVLookup : public ecs::make_ref_from_this<TileUVLookup>
{
public:
  TileUVLookup(TileUVLookup&& other) = default;
  TileUVLookup& operator=(TileUVLookup&&) = default;

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture);

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture, const Rect2i& region);

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture, std::initializer_list<Rect2i> regions);

  ~TileUVLookup();

  inline Size2i tile_size_px() const { return tile_size_px_; }

  inline Size2f tile_size_uv() const { return tile_size_uv_; }

  inline std::size_t tile_count() const { return tile_uv_offsets_.size(); }

  inline const Vec2f& operator[](const std::size_t id) const { return tile_uv_offsets_[id]; }

private:
  /// Size of a tile in pixels
  Size2i tile_size_px_;

  /// Size of tile in UV space
  Size2f tile_size_uv_;

  /// Corner offsets of tiles
  std::vector<Vec2f> tile_uv_offsets_;
};

}  // namespace tyl::graphics
