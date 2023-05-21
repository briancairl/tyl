/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/math/size.hpp>
#include <tyl/utility/alias.hpp>

namespace tyl::graphics
{

/**
 * @brief Position of sprite
 */
using SpritePosition = alias<Vec2f, decltype("SpritePosition"_tag)>;

/**
 * @brief Size of sprite
 */
using SpriteSize = alias<Size2f, decltype("SpriteSize"_tag)>;

/**
 * @brief Options for dividing an atlas texture into sprite frames
 */
class SpriteLookupOptions;

/**
 * @brief Creates a new entity and adds sprite data
 */
ecs::entity
create_sprite(ecs::registry& reg, const Vec2f& position, const Size2f& size, const SpriteLookupOptions& options);

/**
 * @brief Adds sprite data to an existing entity
 */
void attach_sprite(
  ecs::registry& reg,
  const ecs::entity e,
  const Vec2f& position,
  const Size2f& size,
  const SpriteLookupOptions& options);

/**
 * @brief Adds frames to an existing sprite
 */
void update_sprite(ecs::registry& reg, const ecs::entity e, const SpriteLookupOptions& options);

/**
 * @brief Adds single frame to an existing sprite
 */
void update_sprite(ecs::registry& reg, const ecs::entity e, const Rect2f& uv_bounds);

}  // namespace tyl::graphics
