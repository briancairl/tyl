/**
 * @copyright 2022-present Brian Cairl
 *
 * @file alias.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>

namespace tyl
{

/**
 * @brief Strong type alias
 *
 * @tparam T       type to alias
 * @tparam GUID_T  unique identifier type
 * @tparam GUID    unique identifier
 */
template <typename T, typename GUID_T, GUID_T GUID> struct Alias
{
  template <typename... CTorArgs> explicit Alias(CTorArgs&&... ctor_args) : value{std::forward<CTorArgs>(ctor_args)...}
  {}

  constexpr T& operator()() { return value; }
  constexpr const T& operator()() const { return value; }

  constexpr operator T&() { return value; }
  constexpr operator const T&() const { return value; }

  template <typename IndexT> constexpr decltype(auto) operator[](IndexT&& pos)
  {
    return value[std::forward<IndexT>(pos)];
  }

  template <typename IndexT> constexpr decltype(auto) operator[](IndexT&& pos) const
  {
    return value[std::forward<IndexT>(pos)];
  }

  [[no_unique_address]] T value;
};

}  // namespace tyl
