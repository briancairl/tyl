/**
 * @copyright 2022-present Brian Cairl
 *
 * @file animated_sprite.cpp
 */

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/graphics/animated_sprite.hpp>
#include <tyl/graphics/texture_tilesheet_lookup.hpp>

namespace tyl::graphics
{

const Rect2f& get_frame(const AnimatedSpriteFrames& tilesheet, const AnimatedSpriteState& animated_sprite_state)
{
  // Should never be less than minimum progress value
  TYL_ASSERT_GE(animated_sprite_state.progress, AnimatedSpriteState::min_progress);
  TYL_ASSERT_LT(animated_sprite_state.progress, AnimatedSpriteState::max_progress);

  // Since animated_sprite_state.progress in [0, 1), current_tile_index should be [0, tilesheet.size())
  const std::size_t current_tile_index = tilesheet.size() * animated_sprite_state.progress;

  // Get the bounds
  return tilesheet[current_tile_index];
}

void tick_one_shot(
  AnimatedSpriteState& animated_sprite_state,
  const AnimatedSpriteProperties& animated_sprite_properties,
  const float dt)
{
  TYL_ASSERT_GT(animated_sprite_properties.progress_per_second, 0.f);

  animated_sprite_state.progress += animated_sprite_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animated_sprite_state.progress >= AnimatedSpriteState::max_progress)
  {
    animated_sprite_state.progress -= AnimatedSpriteState::max_progress;
  }

  TYL_ASSERT_GE(animated_sprite_state.progress, AnimatedSpriteState::min_progress);
  TYL_ASSERT_LT(animated_sprite_state.progress, AnimatedSpriteState::max_progress);
}

void tick_repeat(
  AnimatedSpriteState& animated_sprite_state,
  const AnimatedSpriteProperties& animated_sprite_properties,
  const float dt)
{
  TYL_ASSERT_GT(animated_sprite_properties.progress_per_second, 0.f);

  animated_sprite_state.progress += animated_sprite_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animated_sprite_state.progress >= AnimatedSpriteState::max_progress)
  {
    animated_sprite_state.progress -= AnimatedSpriteState::max_progress;
  }

  TYL_ASSERT_GE(animated_sprite_state.progress, AnimatedSpriteState::min_progress);
  TYL_ASSERT_LT(animated_sprite_state.progress, AnimatedSpriteState::max_progress);
}

}  // namespace tyl::graphics
