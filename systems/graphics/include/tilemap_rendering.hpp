/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tilemap_rendering.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/ecs/reference.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/texture_management.hpp>

namespace tyl::graphics
{

struct RenderTarget2D;

struct TilemapRendererOptions
{
  /// Total number of tiles that can be rendered
  std::size_t capacity = 100UL;

  /// Texture unit to bind atlas texture to
  int atlas_texture_unit = 0;
};

struct TilemapRendererState
{
  /// Total number of tilemaps currently being rendered on update
  std::size_t size;
};

/**
 * @brief Creates a system which renders static/animated 2D tilemaps
 */
ecs::entity
create_tilemap_renderer(ecs::registry& reg, const TextureReference& texture, const TilemapRendererOptions& options);

/**
 * @brief Attaches components for a system which renders static 2D tilemaps
 */
void attach_tilemap_renderer(ecs::registry& reg, const ecs::entity e, const TilemapRendererOptions& options);

/**
 * @brief Updates 2D tilemap renderers
 */
void update_tilemap_renderers(ecs::registry& reg, const RenderTarget2D& target);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::TilemapRendererOptions</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::TilemapRendererOptions>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::TilemapRendererOptions& options)
  {
    ar& named{"capacity", options.capacity};
    ar& named{"atlas_texture_unit", options.atlas_texture_unit};
  }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::TilemapRendererState</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::TilemapRendererState>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::TilemapRendererState& state) { ar& named{"size", state.size}; }
};

}  // namespace tyl::serialization
