/**
 * @copyright 2021-present Brian Cairl
 *
 * @file actor.cpp
 */

// Tyl
#include <tyl/actor/actor.hpp>
#include <tyl/actor/components.hpp>
#include <tyl/components.hpp>
#include <tyl/graphics/sprite.hpp>

namespace tyl::actor
{

void attach_actor(ecs::registry& registry, const ecs::entity actor_id, const Vec2f& position, const Actions& actions)
{
  registry.emplace<Direction2D>(actor_id, 1.f, 0.f);
  registry.emplace<Motion2D>(actor_id, 0.f, 0.f);
  registry.emplace<Position2D>(actor_id, position);
  registry.emplace<Actions>(actor_id, actions);
}

ecs::entity create_actor(ecs::registry& registry, const Vec2f& position, const Actions& actions)
{
  const auto e = registry.create();
  attach_actor(registry, e, position, actions);
  return e;
}

static constexpr float WALKING_VELOCITY = 20.f;
static constexpr float RUNNING_VELOCITY = 40.f;
static constexpr float WALKING_VELOCITY_SQ = WALKING_VELOCITY * WALKING_VELOCITY;
static constexpr float RUNNING_VELOCITY_SQ = RUNNING_VELOCITY * RUNNING_VELOCITY;

void update(ecs::registry& registry, const duration dt)
{
  const float dt_sec = std::chrono::duration_cast<fseconds>(dt).count();
  const auto view = registry.view<Actions, Position2D, Direction2D, Motion2D>();

  view.each([dt_sec, &registry](auto& actions, auto& position, auto& direction, auto& motion) {
    position += motion * dt_sec;

    const auto prev_sprite_id = actions.get_active();

    // Run
    if (const float v_sq = motion.squaredNorm(); v_sq > RUNNING_VELOCITY_SQ)
    {
      direction = motion;
      actions.active = Actions::RunDown;
    }
    // Walk
    else if (v_sq > WALKING_VELOCITY_SQ)
    {
      direction = motion;
      actions.active = Actions::WalkDown;
    }
    // Rest
    else
    {
      actions.active = Actions::RestDown;
    }

    // Reset motion
    motion.setZero();

    // Direction
    if (std::abs(direction.y()) > std::abs(direction.x()))
    {
      actions.active += (direction.y() > 0.f) ? Actions::ToUp : Actions::ToDown;
    }
    else if (std::abs(direction.y()) < std::abs(direction.x()))
    {
      actions.active += (direction.x() > 0.f) ? Actions::ToLeft : Actions::ToRight;
    }

    // Set active sprite
    const auto curr_sprite_id = actions.get_active();
    if (prev_sprite_id != curr_sprite_id and registry.has<graphics::SpriteSequence>(curr_sprite_id))
    {
      auto& seq = registry.get<graphics::SpriteSequence>(curr_sprite_id);
      seq.period_since_last_update = duration::zero();
    }

    // Enable rendering
    registry.emplace_or_replace<graphics::SpriteRenderingEnabled>(curr_sprite_id);

    // Set sprite position
    registry.get<Rect2D>(curr_sprite_id).snap(position);
  });
}

}  // namespace tyl::actor
