/**
 * @copyright 2021-present Brian Cairl
 *
 * @file array_view.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <type_traits>

namespace tyl
{

/**
 * @brief A non-owning array type
 */
template <typename ElementT> class ArrayView
{
public:
  constexpr ElementT& operator[](const std::size_t index) { return data_[index]; }

  constexpr const ElementT& operator[](const std::size_t index) const { return data_[index]; }

  constexpr std::size_t size() const { return length_; }

  constexpr auto begin() { return data_; }

  constexpr auto end() { return data_ + length_; }

  constexpr auto begin() const { return data_; }

  constexpr auto end() const { return data_ + length_; }

  constexpr ArrayView(ElementT* const data, const std::size_t length) : data_{data}, length_{length} {}

private:
  ElementT* data_;
  std::size_t length_;
};

template <typename ElementT>
constexpr ArrayView<ElementT> make_array_view(ElementT* const ptr, const std::size_t length)
{
  return ArrayView<ElementT>(ptr, length);
}

template <typename ElementT>
constexpr ArrayView<const std::remove_const_t<ElementT>>
make_const_array_view(ElementT* const ptr, const std::size_t length)
{
  return ArrayView<const std::remove_const_t<ElementT>>(ptr, length);
}

template <typename ElementT, std::size_t N> constexpr ArrayView<ElementT> make_array_view(ElementT (&c_array)[N])
{
  return ArrayView<ElementT>(c_array, N);
}

template <typename ElementT, std::size_t N>
constexpr ArrayView<const std::remove_const_t<ElementT>> make_const_array_view(ElementT (&c_array)[N])
{
  return ArrayView<const std::remove_const_t<ElementT>>(c_array, N);
}

}  // namespace tyl
