/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_map.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>
#include <vector>

// Tyl
#include <tyl/common/ecs.hpp>
#include <tyl/common/matrix.hpp>
#include <tyl/common/strong_alias.hpp>
#include <tyl/engine/fwd.hpp>
#include <tyl/graphics/fwd.hpp>

namespace tyl::engine
{

TYL_DEFINE_STRONG_ALIAS(TileAtlasSizePx, Vec2i);

class TileAtlasUVLookup
{
public:
  TileAtlasUVLookup(const TileSizePx& tile_size, const TileAtlasSizePx& tile_altas_size);

  inline const Vec2f& operator[](const std::size_t index) const { return uv_offsets_[index]; }

  inline std::size_t tile_count() const { return uv_offsets_.size(); }

  inline const Vec2f& tile_size_uv() const { return uv_tile_size_; }

private:
  /// Size of tile in UV (texture) space
  Vec2f uv_tile_size_;

  /// Tile offsets in UV space
  std::vector<Vec2f> uv_offsets_;
};

Entity create_tile_map_default_shader(ECSRegistry& registry);

void add_tile_map_render_data(
  ECSRegistry& registry,
  const Entity entity,
  const TileAtlasSizePx& tile_altas_size,
  const graphics::TextureHandle& tile_atlas_tex,
  const graphics::ShaderHandle& tile_map_shader);

void render_tile_maps(ECSRegistry& registry, const ViewProjectionMatrix& view_projection_matrix);

}  // namespace tyl::engine
