/**
 * @copyright 2021-present Brian Cairl
 *
 * @file camera.cpp
 */

// C++ Standard Library
#include <cmath>

// Tyl
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/target.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

inline float aspect_ratio(const Vec2i& vs) { return static_cast<float>(vs.x()) / static_cast<float>(vs.y()); }

}  // namespace anonymous

Mat3f CameraTopDown::get_inverse_view_projection_matrix(const Target& render_target) const
{
  const float rectified_zoom = std::ceil(100 * this->zoom) / 100.f;
  const float rectified_panning_x = std::round(100 * this->panning.x()) / 100.f;
  const float rectified_panning_y = std::round(100 * this->panning.y()) / 100.f;

  Mat3f vp{Mat3f::Zero()};
  vp(0, 0) = rectified_zoom * aspect_ratio(render_target.viewport_size);
  vp(1, 1) = rectified_zoom;
  vp(0, 2) = rectified_panning_x;
  vp(1, 2) = rectified_panning_y;
  vp(2, 2) = 1.0f;
  return vp;
}

}  // namespace tyl::graphics
