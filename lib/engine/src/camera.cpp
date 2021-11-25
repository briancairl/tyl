/**
 * @copyright 2020-present Brian Cairl
 *
 * @file camera.cpp
 */

// C++ Standard Library
#include <cmath>

// Type
#include <tyl/engine/camera.hpp>
#include <tyl/engine/geometry.hpp>
#include <tyl/engine/window.hpp>

namespace tyl::engine
{
namespace  // anonymous
{

inline float aspect_ratio(const ViewportSize& vs) { return static_cast<float>(vs.x()) / static_cast<float>(vs.y()); }

}  // namespace anonymous

InverseViewProjectionMatrix
make_inverse_view_projection_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size)
{
  const float rectified_zoom = std::ceil(100 * camera.zoom) / 100.f;
  const float rectified_panning_x = std::round(100 * camera.panning.x()) / 100.f;
  const float rectified_panning_y = std::round(100 * camera.panning.y()) / 100.f;

  InverseViewProjectionMatrix vp{Mat3f::Zero()};
  vp(0, 0) = rectified_zoom * aspect_ratio(viewport_size);
  vp(1, 1) = rectified_zoom;
  vp(0, 2) = rectified_panning_x;
  vp(1, 2) = rectified_panning_y;
  vp(2, 2) = 1.0f;
  return vp;
}

ViewProjectionMatrix make_view_projection_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size)
{
  return make_inverse_view_projection_matrix(camera, viewport_size).inverse();
}

}  // namespace tyl::engine
