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

using VecXf = Eigen::Matrix<float, Eigen::Dynamic, 1>;
using MatXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;

using Vec2i = Eigen::Matrix<int, 2, 1>;
using Vec3i = Eigen::Matrix<int, 3, 1>;
using Mat2i = Eigen::Matrix<int, 2, 2>;
using Mat3i = Eigen::Matrix<int, 3, 3>;

using VecXi = Eigen::Matrix<int, Eigen::Dynamic, 1>;
using MatXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

}  // namespace tyl
