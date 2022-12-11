/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <array>
#include <cmath>
#include <tuple>

namespace tyl
{
namespace detail
{

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_top_left_corner(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(0)  ,  static_cast<IndexT>(1)  ,  C_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(0)  ,  C_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(1)  ,  I_Dist),
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(0)  ,  C_Dist),
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(1)  ,  I_Dist),
    std::make_tuple(static_cast<IndexT>(0)  ,  static_cast<IndexT>(b)  ,  C_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(b)  ,  I_Dist),
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(b)  ,  I_Dist),
  };
  // clang-format on

  for (const auto& [ni, nj, m] : Offsets)
  {
    kcb(0, 0, ni, nj, m);
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_top_right_corner(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(0  ), I_Dist),
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(a)  ,  static_cast<IndexT>(b  ), C_Dist),
    std::make_tuple(static_cast<IndexT>(0)  ,  static_cast<IndexT>(0  ), C_Dist),
    std::make_tuple(static_cast<IndexT>(0)  ,  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(b-1), C_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(b  ), C_Dist),
    std::make_tuple(static_cast<IndexT>(1)  ,  static_cast<IndexT>(0  ), I_Dist),
  };
  // clang-format on

  for (const auto& [ni, nj, m] : Offsets)
  {
    kcb(0, b, ni, nj, m);
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_bottom_left_corner(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(b)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(a  ),  static_cast<IndexT>(b)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(a  ),  static_cast<IndexT>(1)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(b)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(1)  , I_Dist),
  };
  // clang-format on

  for (const auto& [ni, nj, m] : Offsets)
  {
    kcb(a, 0, ni, nj, m);
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_bottom_right_corner(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(0  ), I_Dist),
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(a-1),  static_cast<IndexT>(b  ), C_Dist),
    std::make_tuple(static_cast<IndexT>(a  ),  static_cast<IndexT>(0  ), C_Dist),
    std::make_tuple(static_cast<IndexT>(a  ),  static_cast<IndexT>(b-1), C_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(0  ), I_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(0  ),  static_cast<IndexT>(b  ), C_Dist),
  };
  // clang-format on

  for (const auto& [ni, nj, m] : Offsets)
  {
    kcb(a, b, ni, nj, m);
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_left_col(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(b)  , I_Dist),
    std::make_tuple(static_cast<IndexT>( 0),  static_cast<IndexT>(b)  , C_Dist),
    std::make_tuple(static_cast<IndexT>( 0),  static_cast<IndexT>(1)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(b)  , I_Dist),
  };
  // clang-format on

  for (IndexT row = 1; row < a; ++row)
  {
    for (const auto& [ndi, ncol, m] : Offsets)
    {
      kcb(row, 0, row + ndi, ncol, m);
    }
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_right_col(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(0  ), I_Dist),
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(b  ), C_Dist),
    std::make_tuple(static_cast<IndexT>( 0),  static_cast<IndexT>(0  ), C_Dist),
    std::make_tuple(static_cast<IndexT>( 0),  static_cast<IndexT>(b-1), C_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(0  ), I_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(b-1), I_Dist),
    std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(b  ), C_Dist),
  };
  // clang-format on

  for (IndexT row = 1; row < a; ++row)
  {
    for (const auto& [ndi, ncol, m] : Offsets)
    {
      kcb(row, b, row + ndi, ncol, m);
    }
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_top_row(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(a), static_cast<IndexT>(-1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(a), static_cast<IndexT>( 0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(a), static_cast<IndexT>(+1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(0), static_cast<IndexT>(-1)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(0), static_cast<IndexT>(+1)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(1), static_cast<IndexT>(-1)  , I_Dist),
    std::make_tuple(static_cast<IndexT>(1), static_cast<IndexT>( 0)  , C_Dist),
    std::make_tuple(static_cast<IndexT>(1), static_cast<IndexT>(+1)  , I_Dist),
  };
  // clang-format on

  for (IndexT col = 1; col < b; ++col)
  {
    for (const auto& [nrow, ndj, m] : Offsets)
    {
      kcb(0, col, nrow, col + ndj, m);
    }
  }
}

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op_bottom_row(const IndexT rows, const IndexT cols, KernelCallbackT&& kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // clang-format off
  const std::array Offsets = {
    std::make_tuple(static_cast<IndexT>(a-1), static_cast<IndexT>(-1),  I_Dist),
    std::make_tuple(static_cast<IndexT>(a-1), static_cast<IndexT>( 0),  C_Dist),
    std::make_tuple(static_cast<IndexT>(a-1), static_cast<IndexT>(+1),  I_Dist),
    std::make_tuple(static_cast<IndexT>(a  ), static_cast<IndexT>(-1),  C_Dist),
    std::make_tuple(static_cast<IndexT>(a  ), static_cast<IndexT>(+1),  C_Dist),
    std::make_tuple(static_cast<IndexT>(0  ), static_cast<IndexT>(-1),  I_Dist),
    std::make_tuple(static_cast<IndexT>(0  ), static_cast<IndexT>( 0),  C_Dist),
    std::make_tuple(static_cast<IndexT>(0  ), static_cast<IndexT>(+1),  I_Dist),
  };
  // clang-format on

  for (IndexT col = 1; col < b; ++col)
  {
    for (const auto& [nrow, ndj, m] : Offsets)
    {
      kcb(a, col, nrow, col + ndj, m);
    }
  }
}

}  // namespace detail

template <typename DistanceT = float, typename IndexT, typename KernelCallbackT>
void kernel_op(IndexT rows, IndexT cols, KernelCallbackT kcb)
{
  static constexpr DistanceT C_Dist = 1.f;
  static constexpr DistanceT I_Dist = std::sqrt(2.f);
  const IndexT a = rows - 1;
  const IndexT b = cols - 1;

  // Scan inner blocks
  {
    // clang-format off
    static constexpr std::array Offsets = {
      std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(-1), I_Dist),
      std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(+0), C_Dist),
      std::make_tuple(static_cast<IndexT>(-1),  static_cast<IndexT>(+1), I_Dist),
      std::make_tuple(static_cast<IndexT>(+0),  static_cast<IndexT>(-1), C_Dist),
      std::make_tuple(static_cast<IndexT>(+0),  static_cast<IndexT>(+1), C_Dist),
      std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(-1), I_Dist),
      std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(+0), C_Dist),
      std::make_tuple(static_cast<IndexT>(+1),  static_cast<IndexT>(+1), I_Dist),
    };
    // clang-format on

    for (IndexT row = 1; row < a; ++row)
    {
      for (IndexT col = 1; col < b; ++col)
      {
        for (const auto& [di, dj, m] : Offsets)
        {
          kcb(row, col, row + di, col + dj, m);
        }
      }
    }
  }

  // Scan outer rows/cols
  detail::kernel_op_left_col<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_right_col<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_top_row<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_bottom_row<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));

  // Scan corners
  detail::kernel_op_top_left_corner<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_top_right_corner<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_bottom_left_corner<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
  detail::kernel_op_bottom_right_corner<DistanceT>(rows, cols, std::forward<KernelCallbackT>(kcb));
}

}  // namespace tyl