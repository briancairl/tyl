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
 * @brief Describes inverted view projection matrix based on camera
 */
TYL_DEFINE_STRONG_ALIAS(InverseViewProjectionMatrix, Mat3f);

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
  float zoom = 100.f;

  TopDownCamera() = default;
};

/**
 * @brief Returns INVERSE view matrix given a camera and viewport
 *
 * @param camera  top-down camera parameters
 * @param viewport_size  current viewport dimensions
 *
 * @return 3x3 view matrix
 */
InverseViewProjectionMatrix
make_inverse_view_projection_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size);

/**
 * @brief Returns view matrix given a camera and viewport
 *
 * @param camera  top-down camera parameters
 * @param viewport_size  current viewport dimensions
 *
 * @return 3x3 view matrix
 */
ViewProjectionMatrix make_view_projection_matrix(const TopDownCamera& camera, const ViewportSize& viewport_size);

/**
 * @brief Returns view matrix given an INVERSE view matrix
 *
 * @param inv_vpm  inverse view projection matrix
 *
 * @return 3x3 view matrix
 */
inline ViewProjectionMatrix make_view_projection_matrix(const InverseViewProjectionMatrix& inv_vpm)
{
  return inv_vpm.inverse();
}

}  // namespace tyl::engine
