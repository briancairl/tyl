/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// Tyl
#include <tyl/common/rect.hpp>

namespace tyl::graphics
{

/**
 * @brief Describes current animation state
 */
struct AnimatedSpriteState
{
  /// Lower-bound value on progress
  static constexpr float min_progress = 0.f;

  /// Upper-bound value on progress
  static constexpr float max_progress = 1.f;

  /// Sprite animation progress, represented as a value between [0, 1)
  float progress = min_progress;

  /// Resets progress state
  constexpr void reset() { progress = min_progress; }
};

/**
 * @brief Describes animation properties
 */
struct AnimatedSpriteProperties
{
  /// Amount to progress animation state per second
  float progress_per_second;
};

// Forward declaration
class TextureTilesheetLookup;

using AnimatedSpriteFrames = TextureTilesheetLookup;

/**
 * @brief Returns bounds for an animation frame in texture UV space
 */
const Rect2f& get_frame(const AnimatedSpriteFrames& tilesheet, const AnimatedSpriteState& animated_sprite_state);

/**
 * @brief Progress sprite animation state
 */
void tick_one_shot(
  AnimatedSpriteState& animated_sprite_state,
  const AnimatedSpriteProperties& animated_sprite_properties,
  const float dt);

/**
 * @brief Progress sprite animation state
 */
void tick_repeat(
  AnimatedSpriteState& animated_sprite_state,
  const AnimatedSpriteProperties& animated_sprite_properties,
  const float dt);

}  // namespace tyl::graphics
