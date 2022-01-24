/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite_renderer.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/fwd.hpp>
#include <tyl/time.hpp>

namespace tyl::graphics
{

struct SpriteRendererProperties
{
  // Maximum number of sprites to be rendered
  std::size_t max_sprite_count;

  // Current number of sprites to be rendered
  std::size_t current_sprite_count;
};

ecs::entity create_sprite_renderer(ecs::registry& registry, const std::size_t max_sprite_count);

void attach_sprite_renderer(ecs::registry& registry, const ecs::entity entity_id, const std::size_t max_sprite_count);

void draw_sprites(ecs::registry& registry, Target& render_target, const duration dt);

}  // namespace tyl::graphics
