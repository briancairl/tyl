/**
 * @copyright 2021-present Brian Cairl
 *
 * @file actor.hpp
 */
#pragma once

// Tyl
#include <tyl/actor/fwd.hpp>
#include <tyl/ecs.hpp>
#include <tyl/time.hpp>
#include <tyl/vec.hpp>

namespace tyl::actor
{

ecs::entity create_actor(ecs::registry& registry, const Vec2f& position, const Actions& actions);

void attach_actor(ecs::registry& registry, const ecs::entity actor_id, const Vec2f& position, const Actions& actions);

void update(ecs::registry& registry, const duration dt);

}  // namespace tyl::actor
