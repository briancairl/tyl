/**
 * @copyright 2021-present Brian Cairl
 *
 * @file map.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/tile_map/components.hpp>
#include <tyl/transform.hpp>

namespace tyl::tile_map
{

ecs::Entity create_default_shader(ecs::Registry& registry);

struct Options
{
  /// Tile map grid dimensions
  Vec2i dimensions;

  /// Tile size in pixels
  Vec2i tile_size;

  /// Initial transform
  Transform transform;

  /// Atlas texture entity ID
  ecs::Entity altas_texture_entity;

  /// Shader entity ID
  ecs::Entity shader_entity;

  /// Initial tile ID value to fill map
  int initial_tile_id;
};

ecs::Entity create(ecs::Registry& registry, const Options& options);

void attach(ecs::Registry& registry, const ecs::Entity tile_map_entity, const Options& options);

}  // namespace tyl::tile_map
