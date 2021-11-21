/**
 * @copyright 2020-present Brian Cairl
 *
 * @file camera.cpp
 */

// Art
#include <tyl/render/camera.hpp>
#include <tyl/render/viewport.hpp>

namespace tyl::render
{
namespace  // anonymous
{

inline float aspect_ratio(const ViewportSize& vs)
{
  return static_cast<float>(vs.width_px) / static_cast<float>(vs.height_px);
}

}  // namespace anonymous

Mat3f make_view_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size)
{
  const float rectified_zoom = std::ceil(100 * camera.zoom) / 100.f;
  const float rectified_panning_x = std::round(100 * camera.panning.x()) / 100.f;
  const float rectified_panning_y = std::round(100 * camera.panning.y()) / 100.f;

  Mat3f cm = Mat3f::Zero();
  cm(0, 0) = rectified_zoom * aspect_ratio(viewport_size);
  cm(1, 1) = -rectified_zoom;
  cm(0, 2) = -rectified_panning_x;
  cm(1, 2) = -rectified_panning_y;
  cm(2, 2) = 1.0f;
  return cm.inverse().eval();
}

}  // namespace tyl::render
