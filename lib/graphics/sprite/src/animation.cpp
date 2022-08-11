/**
 * @copyright 2022-present Brian Cairl
 *
 * @file animation.cpp
 */

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/graphics/sprite/animation.hpp>
#include <tyl/graphics/sprite/spritesheet.hpp>

namespace tyl::graphics::sprite
{

const Rect2f& get_frame(const AnimationFrames& animation_frames, const AnimationState& animation_state)
{
  // Should never be less than minimum progress value
  TYL_ASSERT_GE(animation_state.progress, AnimationState::min_progress);
  TYL_ASSERT_LT(animation_state.progress, AnimationState::max_progress);

  // Since animation_state.progress in [0, 1), current_tile_index should be [0, animation_frames.size())
  const std::size_t current_tile_index = animation_frames.size() * animation_state.progress;

  // Get the bounds
  return animation_frames[current_tile_index];
}

void play_one_shot(AnimationState& animation_state, const AnimationProperties& animation_properties, const float dt)
{
  TYL_ASSERT_GT(animation_properties.progress_per_second, 0.f);

  if (animation_state.complete)
  {
    return;
  }

  const auto prev_progress = animation_state.progress;
  animation_state.progress += animation_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animation_state.progress >= AnimationState::max_progress)
  {
    animation_state.complete = true;
    animation_state.progress = prev_progress;
  }

  TYL_ASSERT_GE(animation_state.progress, AnimationState::min_progress);
  TYL_ASSERT_LT(animation_state.progress, AnimationState::max_progress);
}

void play_repeat(AnimationState& animation_state, const AnimationProperties& animation_properties, const float dt)
{
  TYL_ASSERT_GT(animation_properties.progress_per_second, 0.f);

  animation_state.progress += animation_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animation_state.progress >= AnimationState::max_progress)
  {
    animation_state.progress -= AnimationState::max_progress;
  }

  TYL_ASSERT_GE(animation_state.progress, AnimationState::min_progress);
  TYL_ASSERT_LT(animation_state.progress, AnimationState::max_progress);
}

}  // namespace tyl::graphics::sprite
