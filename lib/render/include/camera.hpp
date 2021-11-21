/**
 * @copyright 2021-present Brian Cairl
 *
 * @file viewport.hpp
 */
#pragma once

// Tyl
#include <tyl/common/matrix.hpp>
#include <tyl/render/fwd.hpp>

namespace tyl::render
{

/**
 * @brief Describes a 2D top-down camera
 */
struct TopDownCamera
{
  Vec2f panning = Vec2f::Zero();

  float zoom = 1.f;

  TopDownCamera() = default;
};

/**
 * @brief Returns view matrix given a camera and viewport
 *
 * @param camera  top-down camera parameters
 * @param viewport_size  current viewport dimensions
 *
 * @return 3x3 view matrix
 */
Mat3f get_view_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size);

}  // namespace tyl::render
