/**
 * @copyright 2021-present Brian Cairl
 *
 * @file vec.hpp
 */
#pragma once

// Eigen
#include <Eigen/Dense>

namespace tyl
{

// clang-format off
using Vec2f = Eigen::Matrix<float, 2, 1>; static_assert(sizeof(Vec2f) == sizeof(float) * 2);
using Vec3f = Eigen::Matrix<float, 3, 1>; static_assert(sizeof(Vec3f) == sizeof(float) * 3);
using Vec4f = Eigen::Matrix<float, 4, 1>; static_assert(sizeof(Vec4f) == sizeof(float) * 4);
using Mat2f = Eigen::Matrix<float, 2, 2>; static_assert(sizeof(Mat2f) == sizeof(float) * 4);
using Mat3f = Eigen::Matrix<float, 3, 3>; static_assert(sizeof(Mat3f) == sizeof(float) * 9);
using VecXf = Eigen::Matrix<float, Eigen::Dynamic, 1>;
using MatXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;

using Vec2d = Eigen::Matrix<double, 2, 1>; static_assert(sizeof(Vec2d) == sizeof(double) * 2);
using Vec3d = Eigen::Matrix<double, 3, 1>; static_assert(sizeof(Vec3d) == sizeof(double) * 3);
using Mat2d = Eigen::Matrix<double, 2, 2>; static_assert(sizeof(Mat2d) == sizeof(double) * 4);
using Mat3d = Eigen::Matrix<double, 3, 3>; static_assert(sizeof(Mat3d) == sizeof(double) * 9);
using VecXd = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using MatXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

using Vec2i = Eigen::Matrix<int, 2, 1>; static_assert(sizeof(Vec2i) == sizeof(int) * 2);
using Vec3i = Eigen::Matrix<int, 3, 1>; static_assert(sizeof(Vec3i) == sizeof(int) * 3);
using Mat2i = Eigen::Matrix<int, 2, 2>; static_assert(sizeof(Mat2i) == sizeof(int) * 4);
using Mat3i = Eigen::Matrix<int, 3, 3>; static_assert(sizeof(Mat3i) == sizeof(int) * 9);
using VecXi = Eigen::Matrix<int, Eigen::Dynamic, 1>;
using MatXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

using Size2i = Eigen::Matrix<int, 2, 1>;
using Size2f = Eigen::Matrix<float, 2, 1>;
// clang-format on

}  // namespace tyl
