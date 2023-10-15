/**
 * @copyright 2022-present Brian Cairl
 *
 * @file size.hpp
 */
#pragma once

// C++ Standard Library
#include <cmath>

// Tyl
#include <tyl/math/vec.hpp>

namespace tyl
{

template <typename T, int Dims> class Size : public Vec<T, Dims>
{
  using BaseType = Vec<T, Dims>;

public:
  explicit Size(const BaseType& extents) : BaseType{extents} {}
};

using Size2i = Size<int, 2>;
using Size2f = Size<float, 2>;

template <int Dims, typename T> inline bool operator==(const Size<T, Dims>& lhs, const Size<T, Dims>& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(Size<T, Dims>)) == 0;
}

template <int Dims, typename T> inline bool operator!=(const Size<T, Dims>& lhs, const Size<T, Dims>& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(Size<T, Dims>)) != 0;
}

}  // namespace tyl
