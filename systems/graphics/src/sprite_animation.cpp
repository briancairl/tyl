/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_animation.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/sprite_animation.hpp>
#include <tyl/math/size.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

void play_one_shot(
  SpriteAnimationState& animation_state,
  const SpriteAnimationProperties& animation_properties,
  const float dt)
{
  TYL_ASSERT_GT(animation_properties.progress_per_second, 0.f);

  if (animation_state.complete)
  {
    return;
  }

  const auto prev_progress = animation_state.progress;
  animation_state.progress += animation_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animation_state.progress >= SpriteAnimationState::max_progress)
  {
    animation_state.complete = true;
    animation_state.progress = prev_progress;
  }

  TYL_ASSERT_GE(animation_state.progress, SpriteAnimationState::min_progress);
  TYL_ASSERT_LT(animation_state.progress, SpriteAnimationState::max_progress);
}

void play_repeat(
  SpriteAnimationState& animation_state,
  const SpriteAnimationProperties& animation_properties,
  const float dt)
{
  TYL_ASSERT_GT(animation_properties.progress_per_second, 0.f);

  animation_state.progress += animation_properties.progress_per_second * dt;

  // Return last frame if over max progress value
  if (animation_state.progress >= SpriteAnimationState::max_progress)
  {
    animation_state.progress -= SpriteAnimationState::max_progress;
  }

  TYL_ASSERT_GE(animation_state.progress, SpriteAnimationState::min_progress);
  TYL_ASSERT_LT(animation_state.progress, SpriteAnimationState::max_progress);
}

void attach_sprite_animation(ecs::registry& reg, const ecs::entity e, const SpriteAnimationProperties& properties)
{
  reg.emplace<SpriteAnimationState>(e);
  reg.emplace<SpriteAnimationProperties>(e, properties);
}

}  // namespace anonymous

void attach_sprite_animation_repeating(
  ecs::registry& reg,
  const ecs::entity e,
  const SpriteAnimationProperties& properties)
{
  reg.emplace<tags::repeat>(e);
  attach_sprite_animation(reg, e, properties);
}

void attach_sprite_animation_one_shot(
  ecs::registry& reg,
  const ecs::entity e,
  const SpriteAnimationProperties& properties)
{
  reg.emplace<tags::one_shot>(e);
  attach_sprite_animation(reg, e, properties);
}

void update_sprite_animations(ecs::registry& reg, const float dt)
{
  {
    auto view = reg.view<tags::rendering_enabled, tags::repeat, SpriteAnimationState, SpriteAnimationProperties>();
    view.each([dt](auto& ani_state, const auto& ani_prop) { play_repeat(ani_state, ani_prop, dt); });
  }

  {
    auto view = reg.view<tags::rendering_enabled, tags::one_shot, SpriteAnimationState, SpriteAnimationProperties>();
    view.each([dt](auto& ani_state, const auto& ani_prop) { play_one_shot(ani_state, ani_prop, dt); });
  }
}

}  // namespace tyl::graphics