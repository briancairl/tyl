/**
 * @copyright 2021-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// Tyl
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/fwd.hpp>
#include <tyl/rect.hpp>
#include <tyl/strong_alias.hpp>
#include <tyl/time.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

TYL_DEFINE_STRONG_ALIAS(ViewportRect, Rect2f)
TYL_DEFINE_STRONG_ALIAS(ViewProjection, Mat3f)
TYL_DEFINE_STRONG_ALIAS(InverseViewProjection, Mat3f)

struct CameraLockedTo
{};

struct CameraTracking
{
  float gain;
};

struct CameraBoundary
{
  float gain;
  float viewport_inset_ratio;
};

/**
 * @brief Describes a 2D top-down camera
 */
struct TopDownCamera
{
  /// Location of the camera
  Vec2f panning = Vec2f::Zero();

  /// Current camera zoom level
  float zoom = 100.f;
};

/**
 * @brief Creates a tile UV lookup resource
 */
ecs::entity create_top_down_camera(
  ecs::registry& registry,
  const Vec2f& initial_panning = Vec2f::Zero(),
  const float initial_zoom = 100.f);

/**
 * @brief Updates camera to lock to another entity
 */
void set_camera_locked_to(const ecs::Ref<TopDownCamera> camera, const ecs::Ref<Position2D> focus_to);

/**
 * @brief Updates camera to dynamically follow to another entity
 */
void set_camera_tracking(
  const ecs::Ref<TopDownCamera> camera,
  const ecs::Ref<Position2D> focus_to,
  const float tracking_gain);

/**
 * @brief Updates camera to dynamically follow to another entity using viewport boundary
 */
void set_camera_boundary(
  const ecs::Ref<TopDownCamera> camera,
  const ecs::Ref<Position2D> focus_to,
  const float tracking_gain,
  const float viewport_inset_ratio);

/**
 * @brief Updates camera matrices and dynamics
 */
void update_cameras(ecs::registry& registry, const Target& render_target, const duration dt);

}  // namespace tyl::graphics
