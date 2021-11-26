/**
 * @copyright 2021-present Brian Cairl
 *
 * @file std_array.hpp
 */
#pragma once

// C++ Standard Library
#include <array>
#include <type_traits>

// Tyl
#include <tyl/common/array_view.hpp>

namespace tyl
{

template <typename ElementT, std::size_t N> constexpr ArrayView<ElementT> make_array_view(std::array<ElementT, N>& vec)
{
  return ArrayView<ElementT>(vec.data(), vec.size());
}

template <typename ElementT, std::size_t N>
constexpr ArrayView<const std::remove_const_t<ElementT>> make_const_array_view(std::array<ElementT, N>& vec)
{
  return ArrayView<const std::remove_const_t<ElementT>>(vec.data(), vec.size());
}

template <typename ElementT, std::size_t N>
constexpr ArrayView<const std::remove_const_t<ElementT>> make_const_array_view(const std::array<ElementT, N>& vec)
{
  return ArrayView<const std::remove_const_t<ElementT>>(vec.data(), vec.size());
}

}  // namespace tyl
