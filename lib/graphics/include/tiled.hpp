/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tiled.hpp
 */
#pragma once

// C++ Standard Library
#include <array>

// Tyl
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/fwd.hpp>
#include <tyl/time.hpp>

namespace tyl::graphics
{

struct TiledRenderingEnabled
{};

struct TiledBatchRenderProperties
{
  // Maximum number of tiles to be rendered
  std::size_t max_tile_count;
};

struct TiledRegion
{
  static constexpr int S = 6;
  static constexpr std::size_t N = S * S;
  std::array<std::size_t, N> ids;
};

ecs::entity create_tiled_batch_renderer(ecs::registry& registry, const std::size_t max_tile_count);

void attach_tiled_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_tile_count);

void draw_tiles(ecs::registry& registry, Target& render_target, const duration dt);

ecs::entity create_tiled(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tiled_rect);

void attach_tiled(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tiled_rect);

}  // namespace tyl::graphics
