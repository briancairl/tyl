/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// Tyl
#include <tyl/common/rect.hpp>

namespace tyl::graphics::sprite
{

/**
 * @brief Describes current animation state
 */
struct AnimationState
{
  /// Lower-bound value on progress
  static constexpr float min_progress = 0.f;

  /// Upper-bound value on progress
  static constexpr float max_progress = 1.f;

  /// Was animation completed
  bool complete = false;

  /// Sprite animation progress, represented as a value between [0, 1)
  float progress = min_progress;

  /// Resets progress state
  constexpr void reset()
  {
    complete = false;
    progress = min_progress;
  }
};

/**
 * @brief Describes animation properties
 */
struct AnimationProperties
{
  /// Amount to progress animation state per second
  float progress_per_second;
};

// Forward declaration
class SpriteSheet;

/// Holds sequnce of frames
using AnimationFrames = SpriteSheet;

/**
 * @brief Returns bounds for an animation frame in texture UV space
 */
const Rect2f& get_frame(const AnimationFrames& animation_frames, const AnimationState& animated_sprite_state);

/**
 * @brief Progress sprite animation state
 */
void play_one_shot(
  AnimationState& animated_sprite_state,
  const AnimationProperties& animated_sprite_properties,
  const float dt);

/**
 * @brief Progress sprite animation state
 */
void play_repeat(
  AnimationState& animated_sprite_state,
  const AnimationProperties& animated_sprite_properties,
  const float dt);

}  // namespace tyl::graphics::sprite
