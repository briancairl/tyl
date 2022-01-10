/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/strong_alias.hpp>
#include <tyl/time.hpp>

namespace tyl::graphics
{

TYL_DEFINE_STRONG_ALIAS(BoundingBoxColor, Vec4f)

struct BoundingBoxBatchRenderProperties
{
  // Maximum number of rectangles to be rendered
  std::size_t max_bounding_box_count;
};

ecs::entity create_bounding_box_batch_renderer(ecs::registry& registry, const std::size_t max_bounding_box_count);

void attach_bounding_box_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_bounding_box_count);

void draw_bounding_boxes(ecs::registry& registry, Target& render_target, const duration dt);

}  // namespace tyl::graphics
