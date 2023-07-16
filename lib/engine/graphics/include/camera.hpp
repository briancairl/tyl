/**
 * @copyright 2023-present Brian Cairl
 *
 * @file primitives_renderer_types.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/math/vec.hpp>

namespace tyl::engine::graphics
{

struct TopDownCamera2D
{
  Vec2f translation = {0.f, 0.f};
  float scaling = 1.f;
  float aspect_ratio = 1.f;
};

using CameraMatrix2D = Mat3f;

inline Mat3f to_camera_matrix(const TopDownCamera2D& camera)
{
  const float rxx = camera.aspect_ratio * camera.scaling;
  const float ryy = camera.scaling;
  Mat3f m;
  m << rxx, 0.f, camera.translation.x(), 0.f, ryy, camera.translation.y(), 0.f, 0.f, 1.f;
  return m.inverse();
}

}  // namespace tyl::engine::graphics
