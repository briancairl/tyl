/**
 * @copyright 2021-present Brian Cairl
 *
 * @file components.hpp
 */
#pragma once

// C++ Standard Library
#include <array>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/strong_alias.hpp>
#include <tyl/vec.hpp>

namespace tyl::actor
{

TYL_DEFINE_STRONG_ALIAS(Direction2D, Vec2f);
TYL_DEFINE_STRONG_ALIAS(Motion2D, Vec2f);

struct Actions
{
  enum Type
  {
    RestDown,
    RestUp,
    RestLeft,
    RestRight,
    WalkDown,
    WalkUp,
    WalkLeft,
    WalkRight,
    RunDown,
    RunUp,
    RunLeft,
    RunRight,
    _N
  };

  static constexpr std::size_t Count = static_cast<std::size_t>(Type::_N);
  static constexpr std::size_t ToDown = static_cast<std::size_t>(RestDown - RestDown);
  static constexpr std::size_t ToUp = static_cast<std::size_t>(RestUp - RestDown);
  static constexpr std::size_t ToLeft = static_cast<std::size_t>(RestLeft - RestDown);
  static constexpr std::size_t ToRight = static_cast<std::size_t>(RestRight - RestDown);

  std::array<ecs::entity, Count> sprites;

  std::size_t active = RestDown;

  inline ecs::entity get_active() const { return sprites[active]; }

  Actions() = default;

  explicit Actions(std::array<ecs::entity, Count> _sprites) : sprites{_sprites}, active{RestDown} {}
};

}  // namespace tyl::actor
