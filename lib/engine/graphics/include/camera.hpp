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

inline Mat3f to_camera_matrix(const TopDownCamera2D& camera, const float viewport_height, const float viewport_width)
{
  const float rxx = camera.scaling * viewport_height / viewport_width;
  const float ryy = camera.scaling;
  const float dx = camera.translation.x();
  const float dy = camera.translation.y();
  Mat3f m;
  // clang-format off
  m << rxx, 0.f, -dx,
       0.f, ryy, +dy,
       0.f, 0.f, 1.f;
  // clang-format on
  return m.inverse();
}

}  // namespace tyl::engine::graphics
