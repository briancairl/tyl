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
class TileUVLookup : public ecs::make_handle_from_this<TileUVLookup>
{
public:
  TileUVLookup(TileUVLookup&& other) = default;
  TileUVLookup& operator=(TileUVLookup&&) = default;

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture);

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture, const Rect2i& region);

  TileUVLookup(const Size2i tile_size_px, const Texture& atlas_texture, std::initializer_list<Rect2i> regions);

  ~TileUVLookup();

  inline std::size_t tile_count() const { return tile_uv_offsets_.size(); }

  /**
   * @brief Returns tile UV offset as <code>{lower corner [0, 1], upper corner [2, 3]}</code>
   */
  inline const Vec4f& operator[](const std::size_t id) const { return tile_uv_offsets_[id]; }

private:
  /// Corner offsets of tiles
  std::vector<Vec4f> tile_uv_offsets_;
};

}  // namespace tyl::graphics
