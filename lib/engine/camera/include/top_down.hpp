/**
 * @copyright 2021-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// Tyl
#include <tyl/matrix.hpp>

namespace tyl::camera
{

/**
 * @brief Describes a 2D top-down camera
 */
struct TopDown
{
  /// Location of the camera
  Vec2f panning = Vec2f::Zero();

  /// Current camera zoom level
  float zoom = 100.f;

  /**
   * @brief Returns INVERSE view matrix given a camera and viewport
   *
   * @param viewport_size  current viewport dimensions
   *
   * @return 3x3 view matrix
   */
  Mat3f get_inverse_view_projection_matrix(const Vec2i& viewport_size) const;

  /**
   * @brief Returns view matrix given a camera and viewport
   *
   * @param viewport_size  current viewport dimensions
   *
   * @return 3x3 view matrix
   */
  inline Mat3f get_view_projection_matrix(const Vec2i& viewport_size) const
  {
    return get_inverse_view_projection_matrix(viewport_size).inverse();
  }

  TopDown() = default;
};

}  // namespace tyl::camera
