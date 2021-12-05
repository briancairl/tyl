/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_uv_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/graphics/fwd.hpp>
#include <tyl/rect.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

Vec2f rectified_uv_extents(const Size2i tile_size_px, const Texture& atlas_texture);

struct UniformlyDividedRegion
{
  Vec2i subdivisions;
  Vec2i inner_padding_px;
  Rect2i area_px;
  bool reversed;
};

/**
 * @brief Lookup from ID to UV offset in an altas texture
 */
class TileUVLookup : public ecs::make_handle_from_this<TileUVLookup>
{
public:
  TileUVLookup() = default;

  TileUVLookup(TileUVLookup&& other) = default;

  TileUVLookup& operator=(TileUVLookup&&) = default;

  TileUVLookup(const Texture& atlas_texture, const Rect2i& region);

  TileUVLookup(const Texture& atlas_texture, const UniformlyDividedRegion& region);

  ~TileUVLookup();

  inline std::size_t tile_count() const { return tile_uv_offsets_.size(); }

  /**
   * @brief Returns tile UV offset as <code>{lower corner [0, 1], upper corner [2, 3]}</code>
   */
  inline const Vec4f& operator[](const std::size_t id) const { return tile_uv_offsets_[id]; }

  void update(const Texture& atlas_texture, const Rect2i& regions);

  void update(const Texture& atlas_texture, const UniformlyDividedRegion& regions);

private:
  /// Corner offsets of tiles
  std::vector<Vec4f> tile_uv_offsets_;
};

}  // namespace tyl::graphics
