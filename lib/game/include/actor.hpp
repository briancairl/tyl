/**
 * @copyright 2021-present Brian Cairl
 *
 * @file actor.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/game/fwd.hpp>
#include <tyl/strong_alias.hpp>
#include <tyl/time.hpp>
#include <tyl/vec.hpp>

namespace tyl::game
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

ecs::entity create_actor(ecs::registry& registry, const Vec2f& position, const Actions& actions);

void attach_actor(ecs::registry& registry, const ecs::entity actor_id, const Vec2f& position, const Actions& actions);

void update_actors(ecs::registry& registry, const duration dt);

}  // namespace tyl::game
