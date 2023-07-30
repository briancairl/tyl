/**
 * @copyright 2023-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// Tyl
#include <tyl/math/vec.hpp>

namespace tyl::engine::graphics
{

struct TopDownCamera2D
{
  Vec2f translation = {0.f, 0.f};
  float scaling = 1.f;
};

using CameraMatrix2D = Mat3f;

inline Mat3f to_camera_inverse_matrix(const TopDownCamera2D& camera, const Vec2f& viewport_size)
{
  const float rxx = camera.scaling * viewport_size.x() / viewport_size.y();
  const float ryy = camera.scaling;
  const float dx = camera.translation.y();
  const float dy = camera.translation.x();
  Mat3f m;
  // clang-format off
  m << rxx, 0.f, -dx,
       0.f, ryy, +dy,
       0.f, 0.f, 1.f;
  // clang-format on
  return m;
}

inline Mat3f to_camera_matrix(const TopDownCamera2D& camera, const Vec2f& viewport_size)
{
  return to_camera_inverse_matrix(camera, viewport_size).inverse();
}

}  // namespace tyl::engine::graphics
