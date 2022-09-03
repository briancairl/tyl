/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_animation.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs/ecs.hpp>

namespace tyl::graphics
{
namespace tags
{

struct repeat
{};

struct one_shot
{};

}  // namespace tags

/**
 * @brief Describes current animation state
 */
struct SpriteAnimationState
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
struct SpriteAnimationProperties
{
  /// Amount to progress animation state per second
  float progress_per_second;
};

/**
 * @brief Adds sprite animation data to an existing entity
 */
void attach_sprite_animation_repeating(
  ecs::registry& reg,
  const ecs::entity e,
  const SpriteAnimationProperties& properties);

/**
 * @brief Adds sprite animation data to an existing entity
 */
void attach_sprite_animation_one_shot(
  ecs::registry& reg,
  const ecs::entity e,
  const SpriteAnimationProperties& properties);

/**
 * @brief Updates 2D sprite animations
 */
void update_sprite_animations(ecs::registry& reg, const float dt);

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>
#include <tyl/serial/math/rect.hpp>
#include <tyl/serial/math/vec.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::tags::repeat</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::tags::repeat>
{
  constexpr void operator()(ArchiveT& ar, ::tyl::graphics::tags::repeat& _tag_) {}
};

/**
 * @brief Archive-generic <code>::tyl::graphics::tags::one_shot</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::tags::one_shot>
{
  constexpr void operator()(ArchiveT& ar, ::tyl::graphics::tags::one_shot& _tag_) {}
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteAnimationState</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteAnimationState>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteAnimationState& state)
  {
    ar& named{"complete", state.complete};
    ar& named{"progress", state.progress};
  }
};

/**
 * @brief Archive-generic <code>::tyl::graphics::SpriteAnimationProperties</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::SpriteAnimationProperties>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::SpriteAnimationProperties& properties)
  {
    ar& named{"progress_per_second", properties.progress_per_second};
  }
};

}  // namespace tyl::serialization
