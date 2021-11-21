/**
 * @copyright 2021-present Brian Cairl
 *
 * @file matrix.hpp
 */
#pragma once

// Eigen
#include <Eigen/Dense>

namespace tyl
{

using Vec2f = Eigen::Matrix<float, 2, 1>;
using Vec3f = Eigen::Matrix<float, 3, 1>;
using Mat2f = Eigen::Matrix<float, 2, 2>;
using Mat3f = Eigen::Matrix<float, 3, 3>;

}  // namespace tyl
