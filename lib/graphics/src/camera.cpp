/**
 * @copyright 2021-present Brian Cairl
 *
 * @file camera.cpp
 */

// C++ Standard Library
#include <cmath>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/target.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

inline float aspect_ratio(const Vec2i& vs) { return static_cast<float>(vs.x()) / static_cast<float>(vs.y()); }

inline Mat3f get_inverse_view_projection_matrix(const TopDownCamera& camera, const Target& render_target)
{
  Mat3f vp{Mat3f::Zero()};
  vp(0, 0) = camera.zoom * aspect_ratio(render_target.viewport_size);
  vp(1, 1) = camera.zoom;
  vp(0, 2) = camera.panning.x();
  vp(1, 2) = camera.panning.y();
  vp(2, 2) = 1.0f;
  return vp;
}

void reset_tracking(const ecs::Ref<TopDownCamera>& camera)
{
  camera.remove_if_exists<CameraLockedTo, CameraTracking, CameraBoundary, ecs::Ref<Position2D>>();
}

}  // namespace anonymous

ecs::entity create_top_down_camera(ecs::registry& registry, const Vec2f& initial_panning, const float initial_zoom)
{
  const auto e = registry.create();
  registry.emplace<TopDownCamera>(e, initial_panning, initial_zoom);
  registry.emplace<ViewProjection>(e, Mat3f::Identity());
  registry.emplace<InverseViewProjection>(e, Mat3f::Identity());
  registry.emplace<ViewportRect>(e);
  return e;
}

void set_camera_locked_to(const ecs::Ref<TopDownCamera> camera, const ecs::Ref<Position2D> focus_to)
{
  reset_tracking(camera);
  camera.emplace<ecs::Ref<Position2D>>(focus_to);
  camera.emplace<CameraLockedTo>();
}

void set_camera_tracking(
  const ecs::Ref<TopDownCamera> camera,
  const ecs::Ref<Position2D> focus_to,
  const float tracking_gain)
{
  TYL_ASSERT_GT(tracking_gain, 0.f);

  reset_tracking(camera);
  camera.emplace<ecs::Ref<Position2D>>(focus_to);
  camera.emplace<CameraTracking>(tracking_gain);
}

void set_camera_boundary(
  const ecs::Ref<TopDownCamera> camera,
  const ecs::Ref<Position2D> focus_to,
  const float tracking_gain,
  const float viewport_inset_ratio)
{
  TYL_ASSERT_GT(viewport_inset_ratio, 0.f);
  TYL_ASSERT_LT(viewport_inset_ratio, 1.f);

  reset_tracking(camera);
  camera.emplace<ecs::Ref<Position2D>>(focus_to);
  camera.emplace<CameraBoundary>(tracking_gain, viewport_inset_ratio);
}

void update_cameras(ecs::registry& registry, const Target& render_target, const duration dt)
{
  const float dt_sec = std::chrono::duration_cast<fseconds>(dt).count();

  // Update locked followers
  registry.view<CameraLockedTo, TopDownCamera, ecs::Ref<Position2D>>().each(
    [](auto& camera, const auto& focused_position_ref) { camera.panning = *focused_position_ref; });

  // Update tracking followers
  registry.view<CameraTracking, TopDownCamera, ecs::Ref<Position2D>>().each(
    [dt_sec](const auto& tracking_spec, auto& camera, const auto& focused_position_ref) {
      const Vec2f delta = (*focused_position_ref - camera.panning);
      const float dt_gain = std::min(1.f, tracking_spec.gain * dt_sec);
      camera.panning += delta * dt_gain;
    });

  // Update boundary followers
  registry.view<CameraBoundary, TopDownCamera, ecs::Ref<Position2D>, ViewportRect>().each(
    [dt_sec](const auto& boundary_spec, auto& camera, const auto& focused_position_ref, const auto& viewport_rect) {
      const Rect2f inset_rect = Rect2f::scaled_from_center(viewport_rect, 1.f - boundary_spec.viewport_inset_ratio);
      const Vec2f min_corner = inset_rect.min();
      const Vec2f max_corner = inset_rect.max();

      const Position2D& focused_position = *focused_position_ref;

      const float dt_gain = std::min(1.f, boundary_spec.gain * dt_sec);
      camera.panning.x() -= std::max(min_corner.x() - focused_position.x(), 0.f) * dt_gain;
      camera.panning.y() -= std::max(min_corner.y() - focused_position.y(), 0.f) * dt_gain;
      camera.panning.x() += std::max(focused_position.x() - max_corner.x(), 0.f) * dt_gain;
      camera.panning.y() += std::max(focused_position.y() - max_corner.y(), 0.f) * dt_gain;
    });

  // Update viewport properties
  registry.view<TopDownCamera, ViewProjection, InverseViewProjection, ViewportRect>().each(
    [&render_target](const auto& camera, auto& vp, auto& ivp, auto& viewport_rect) {
      static const Vec2f S_screen_device_lower_corner{-1, -1};
      static const Vec2f S_screen_device_upper_corner{+1, +1};

      ivp = get_inverse_view_projection_matrix(camera, render_target);

      viewport_rect = Rect2f::corners(
        ivp.template block<2, 2>(0, 0) * S_screen_device_lower_corner + ivp.template block<2, 1>(0, 2),
        ivp.template block<2, 2>(0, 0) * S_screen_device_upper_corner + ivp.template block<2, 1>(0, 2));

      vp = ivp.inverse();
    });
}

}  // namespace tyl::graphics
