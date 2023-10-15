/**
 * @copyright 2022-present Brian Cairl
 *
 * @file vec.hpp
 */
#pragma once

// C++ Standard Library
#include <cmath>

// Eigen
#include <Eigen/Dense>

namespace tyl
{

template <typename T, int N, int M> using Mat = Eigen::Matrix<T, N, M>;

template <typename T, int N> using Vec = Mat<T, N, 1>;

using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec4i = Vec<int, 4>;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

using Mat2i = Mat<int, 2, 2>;
using Mat3i = Mat<int, 3, 3>;
using Mat4i = Mat<int, 4, 4>;
using MatXi = Mat<int, Eigen::Dynamic, Eigen::Dynamic>;
using Mat2f = Mat<float, 2, 2>;
using Mat3f = Mat<float, 3, 3>;
using Mat4f = Mat<float, 4, 4>;

template <typename TargetT, typename MinT, typename MaxT>
inline void clamp(TargetT&& target, const Eigen::DenseBase<MinT>& min_values, const Eigen::DenseBase<MaxT>& max_values)
{
  static_assert(
    static_cast<int>(std::remove_reference_t<TargetT>::SizeAtCompileTime) == static_cast<int>(MinT::SizeAtCompileTime));
  static_assert(
    static_cast<int>(std::remove_reference_t<TargetT>::SizeAtCompileTime) == static_cast<int>(MaxT::SizeAtCompileTime));
  for (int i = 0; i < std::remove_reference_t<TargetT>::SizeAtCompileTime; ++i)
  {
    target[i] = std::min(std::max(target[i], min_values[i]), max_values[i]);
  }
}

template <typename TargetT, typename MinT, typename MaxT>
inline TargetT
clamped(TargetT&& target, const Eigen::DenseBase<MinT>& min_values, const Eigen::DenseBase<MaxT>& max_values)
{
  std::remove_reference_t<TargetT> retval{std::forward<TargetT>(target)};
  clamp(retval, min_values, max_values);
  return retval;
}

template <typename AsT, typename OrigT, int Access> inline AsT& as(Eigen::MapBase<OrigT, Access>& v)
{
  static_assert(alignof(AsT) == alignof(typename OrigT::Scalar));
  return *reinterpret_cast<AsT*>(v.data());
}

template <typename AsT, typename OrigT, int Access> inline const AsT& as(const Eigen::MapBase<OrigT, Access>& v)
{
  static_assert(alignof(AsT) == alignof(typename OrigT::Scalar));
  return *reinterpret_cast<const AsT*>(v.data());
}

}  // namespace tyl
