/**
 * @copyright 2021-present Brian Cairl
 *
 * @file camera.hpp
 */
#pragma once

// Tyl
#include <tyl/common/matrix.hpp>
#include <tyl/common/strong_alias.hpp>
#include <tyl/engine/fwd.hpp>

namespace tyl::engine
{

/**
 * @brief Describes view projection matrix based on camera
 */
TYL_DEFINE_STRONG_ALIAS(ViewProjectionMatrix, Mat3f);

/**
 * @brief Describes a 2D top-down camera
 */
struct TopDownCamera
{
  Vec2f panning = Vec2f::Zero();

  float zoom = 1.f;

  TopDownCamera() = default;
};

/**
 * @brief Returns INVERSE view matrix given a camera and viewport
 *
 * @param camera  top-down camera parameters
 * @param viewport_size  current viewport dimensions
 * @param unit_conversion  specifies pixel-to-real unit conversions
 *
 * @return 3x3 view matrix
 */
ViewProjectionMatrix make_inverse_view_projection_matrix(
  const TopDownCamera& camera,
  const ViewportSize& viewport_size,
  const UnitConversion& unit_conversion);


}  // namespace tyl::engine
