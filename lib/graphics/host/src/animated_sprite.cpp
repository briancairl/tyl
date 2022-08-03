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

const Rect2f&
get_animation_frame_bounds(const TextureTilesheetLookup& tilesheet, const AnimatedSpriteState& animated_sprite_state)
{
  // Should never be less than minimum progress value
  TYL_ASSERT_GE(animated_sprite_state.progress, AnimatedSpriteState::min_progress);

  // Return last frame if over max progress value
  if (animated_sprite_state.progress >= AnimatedSpriteState::max_progress)
  {
    TYL_ASSERT_GT(tilesheet.tile_count(), 0);
    return tilesheet[tilesheet.tile_count() - 1];
  }

  TYL_ASSERT_LT(animated_sprite_state.progress, AnimatedSpriteState::max_progress);

  // Since animated_sprite_state.progress in [0, 1), current_tile_index should be [0, tilesheet.tile_count())
  const std::size_t current_tile_index = tilesheet.tile_count() * animated_sprite_state.progress;

  // Get the bounds
  return tilesheet[current_tile_index];
}

void tick_animation(
  AnimatedSpriteState& animated_sprite_state,
  const AnimatedSpriteProperties& animated_sprite_properties,
  const float dt)
{
  TYL_ASSERT_GT(animated_sprite_properties.progress_per_second, 0.f);
  animated_sprite_state.progress += animated_sprite_properties.progress_per_second * dt;
}

}  // namespace tyl::graphics
