/**
 * @copyright 2020-present Brian Cairl
 *
 * @file camera.cpp
 */

// C++ Standard Library
#include <cmath>

// Tyl
#include <tyl/camera/top_down.hpp>

namespace tyl::camera
{
namespace  // anonymous
{

inline float aspect_ratio(const Vec2i& vs) { return static_cast<float>(vs.x()) / static_cast<float>(vs.y()); }

}  // namespace anonymous

Mat3f TopDown::get_inverse_view_projection_matrix(const Vec2i& viewport_size) const
{
  const float rectified_zoom = std::ceil(100 * this->zoom) / 100.f;
  const float rectified_panning_x = std::round(100 * this->panning.x()) / 100.f;
  const float rectified_panning_y = std::round(100 * this->panning.y()) / 100.f;

  Mat3f vp{Mat3f::Zero()};
  vp(0, 0) = rectified_zoom * aspect_ratio(viewport_size);
  vp(1, 1) = rectified_zoom;
  vp(0, 2) = rectified_panning_x;
  vp(1, 2) = rectified_panning_y;
  vp(2, 2) = 1.0f;
  return vp;
}

}  // namespace tyl::camera
