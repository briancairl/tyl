/**
 * @copyright 2022-present Brian Cairl
 *
 * @file spritesheet.hpp
 */
#pragma once

// Tyl
#include <tyl/common/serialization/math/rect.hpp>
#include <tyl/common/serialization/std/vector.hpp>
#include <tyl/graphics/sprite/spritesheet.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>graphics::sprite::SpriteSheet</code> save implementation
 */
template <typename ArchiveT> struct save<ArchiveT, graphics::sprite::SpriteSheet>
{
  void operator()(ArchiveT& ar, const graphics::sprite::SpriteSheet& spritesheet)
  {
    ar << named{"bounds", spritesheet.bounds()};
  }
};

/**
 * @brief Archive-generic <code>graphics::sprite::SpriteSheet</code> load implementation
 */
template <typename ArchiveT> struct load<ArchiveT, graphics::sprite::SpriteSheet>
{
  void operator()(ArchiveT& ar, graphics::sprite::SpriteSheet& spritesheet)
  {
    auto bounds = spritesheet.bounds();
    ar >> named{"bounds", bounds};
    spritesheet = std::move(bounds);
  }
};

}  // namespace tyl::serialization
