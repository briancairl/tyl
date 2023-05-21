/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_animation.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/graphics/sprite_lookup.hpp>
#include <tyl/graphics/texture_management.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Describes tilemap properties
 */
struct TileMapProperties
{
  /// Height of tilemap along x-axis
  int rows;

  /// Width of tilemap along y-axis
  int cols;

  /// Side length of a tile
  float tile_size;
};

/**
 * @brief Adds sprite animation data to an existing entity
 */
void attach_tilemap(ecs::registry& reg, const ecs::entity e, const Vec2f& origin, const TileMapProperties& properties);

/**
 * @brief Returns rectangle bounding the closest tile
 */
std::optional<Rect2f> get_tile_rect(const ecs::registry& reg, const Vec2f& query);

}  // namespace tyl::graphics
