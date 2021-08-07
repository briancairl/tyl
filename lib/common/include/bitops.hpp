/**
 * @copyright 2021-present Brian Cairl
 *
 * @file filesystem.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

namespace tyl
{

template <typename T> constexpr T bitmask(const std::size_t offset) { return (static_cast<T>(1) << offset); }

template <typename MaskT> constexpr bool is_one_hot(MaskT m) { return m and !(m & (m - 1)); }

template <typename MaskT> constexpr MaskT count_bits_set(const MaskT n)
{
  return (n == 0) ? 0 : ((n & 1) + count_bits_set(n >> 1));
}

}  // namespace tyl
