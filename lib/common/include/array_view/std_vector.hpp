/**
 * @copyright 2021-present Brian Cairl
 *
 * @file std_vector.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <vector>

// Tyl
#include <tyl/common/array_view.hpp>

namespace tyl
{

template <typename ElementT, typename AllocatorT>
constexpr ArrayView<ElementT> make_array_view(std::vector<ElementT, AllocatorT>& vec)
{
  return ArrayView<ElementT>(vec.data(), vec.size());
}

template <typename ElementT, typename AllocatorT>
constexpr ArrayView<const std::remove_const_t<ElementT>> make_const_array_view(std::vector<ElementT, AllocatorT>& vec)
{
  return ArrayView<const std::remove_const_t<ElementT>>(vec.data(), vec.size());
}

template <typename ElementT, typename AllocatorT>
constexpr ArrayView<const std::remove_const_t<ElementT>>
make_const_array_view(const std::vector<ElementT, AllocatorT>& vec)
{
  return ArrayView<const std::remove_const_t<ElementT>>(vec.data(), vec.size());
}

}  // namespace tyl
