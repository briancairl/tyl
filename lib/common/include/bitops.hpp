/**
 * @copyright 2021-present Brian Cairl
 *
 * @file bitops.hpp
 */
#pragma once

// C++ Standard Library
#include <limits>
#include <type_traits>
#include <utility>

namespace tyl::bitops
{

template <typename T> constexpr T make_mask(const std::size_t offset) { return (static_cast<T>(1) << offset); }

template <typename T, std::size_t Offset> constexpr T make_mask() { return (static_cast<T>(1) << Offset); }

template <typename T> constexpr T make_mask_first_n(const std::size_t n)
{
  return (std::numeric_limits<T>::max() >> (sizeof(T) * 8UL - n));
}

template <typename T, std::size_t N> constexpr T make_mask_first_n()
{
  return (std::numeric_limits<T>::max() >> (sizeof(T) * 8UL - N));
}

template <typename T> constexpr bool is_one_hot(T m) { return m and !(m & (m - 1)); }

template <typename T> constexpr T count(const T n) { return (n == 0) ? 0 : ((n & 1) + count(n >> 1)); }

template <typename MaskT, typename IndexT> constexpr void set(MaskT&& mask, const IndexT index)
{
  std::forward<MaskT>(mask) |= make_mask<std::remove_reference_t<MaskT>>(index);
}

template <typename MaskT, typename IndexT> constexpr void clear(MaskT&& mask, const IndexT index)
{
  std::forward<MaskT>(mask) &= ~make_mask<std::remove_reference_t<MaskT>>(index);
}

template <typename MaskT, typename IndexT> constexpr void clear(MaskT&& mask) { std::forward<MaskT>(mask) = 0; }

template <typename MaskT> constexpr bool any(MaskT&& mask) { return std::forward<MaskT>(mask) != 0; }

template <typename MaskT, typename IndexT> constexpr bool check(MaskT&& mask, const IndexT index)
{
  return std::forward<MaskT>(mask) & make_mask<std::remove_reference_t<MaskT>>(index);
}

}  // namespace tyl::bitops
