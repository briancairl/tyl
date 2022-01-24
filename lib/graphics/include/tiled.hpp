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

namespace tyl::graphics
{

struct TiledRegion
{
  static constexpr int S = 6;
  static constexpr std::size_t N = S * S;
  std::array<std::size_t, N> ids;
};

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
