/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_uv_lookup.hpp
 */
#pragma once

// C++ Standard Library
#include <initializer_list>
#include <variant>
#include <vector>

// Tyl
#include <tyl/ecs.hpp>
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
class TileUVLookup
{
public:
  TileUVLookup() = default;

  TileUVLookup(TileUVLookup&& other) = default;

  TileUVLookup& operator=(TileUVLookup&&) = default;

  TileUVLookup(const TileUVLookup& reference, std::initializer_list<std::size_t> subset_indices);

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

using TextureRegion = std::variant<Rect2i, UniformlyDividedRegion>;

using TileUVLookupAssetsRef = ecs::Ref<TileUVLookup, ecs::Ref<Texture>>;

/**
 * @brief Gets reference to TileUVLookup and associated assets
 */
inline TileUVLookupAssetsRef ref_tile_uv_lookup(ecs::registry& registry, const ecs::entity entity_id)
{
  return ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, entity_id);
}

/**
 * @brief Creates a tile UV lookup resource
 */
ecs::entity create_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::Ref<Texture> texture,
  const std::initializer_list<TextureRegion>& regions);

/**
 * @brief Attaches a tile UV lookup resource components to an existing entity
 */
void attach_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<Texture> texture,
  const std::initializer_list<TextureRegion>& regions);

/**
 * @brief Creates a tile UV lookup resource
 */
ecs::entity create_tile_uv_lookup(
  ecs::registry& registry,
  const TileUVLookupAssetsRef reference,
  const std::initializer_list<std::size_t> subset_indices);

/**
 * @brief Attaches a tile UV lookup resource components to an existing entity
 */
void attach_tile_uv_lookup(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const TileUVLookupAssetsRef reference,
  const std::initializer_list<std::size_t> subset_indices);

}  // namespace tyl::graphics
