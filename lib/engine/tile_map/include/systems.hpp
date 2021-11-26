/**
 * @copyright 2021-present Brian Cairl
 *
 * @file systems.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/matrix.hpp>

namespace tyl::tile_map
{

void render(ecs::Registry& registry, const Mat3f& view_projection_matrix);

}  // namespace tyl::tile_map
