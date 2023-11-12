/**
 * @copyright 2023-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/math.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::engine
{

struct TopDownCamera2D
{
  Vec2f translation = {0.f, 0.f};
  float scaling = 1.f;
  Vec2f viewport_size = {0.f, 0.f};
};

inline Mat3f ToInverseCameraMatrix(const TopDownCamera2D& camera)
{
  const float rxx = camera.scaling * camera.viewport_size.x() / camera.viewport_size.y();
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

inline Mat3f ToCameraMatrix(const TopDownCamera2D& camera) { return ToInverseCameraMatrix(camera).inverse(); }

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::TopDownCamera2D> : std::true_type
{};

}  // namespace tyl::serialization
