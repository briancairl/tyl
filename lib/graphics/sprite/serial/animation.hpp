/**
 * @copyright 2022-present Brian Cairl
 *
 * @file animation.hpp
 */
#pragma once

// Tyl
#include <tyl/graphics/sprite/animation.hpp>
#include <tyl/serial/std/vector.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>graphics::sprite::AnimationState</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, graphics::sprite::AnimationState>
{
  void operator()(ArchiveT& ar, graphics::sprite::AnimationState& animation_state)
  {
    ar& named{"complete", animation_state.complete};
    ar& named{"progress", animation_state.progress};
  }
};

/**
 * @brief Archive-generic <code>graphics::sprite::AnimationProperties</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, graphics::sprite::AnimationProperties>
{
  void operator()(ArchiveT& ar, graphics::sprite::AnimationProperties& animation_properties)
  {
    ar& named{"progress_per_second", animation_properties.progress_per_second};
  }
};

}  // namespace tyl::serialization
