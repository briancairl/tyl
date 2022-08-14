/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::serialization::binary
{
namespace detail
{

template <typename VoidT> struct payload
{
  std::size_t len;
  VoidT* data;
};

template <std::size_t SIZE, typename VoidT> struct payload_fixed_size
{
  static constexpr std::size_t len = SIZE;
  VoidT* data;
};

}  // namespace detail

struct payload : detail::payload<void>
{};

struct const_payload : detail::payload<const void>
{};

template <std::size_t SIZE> struct payload_fixed_size : detail::payload_fixed_size<SIZE, void>
{};

template <std::size_t SIZE> struct const_payload_fixed_size : detail::payload_fixed_size<SIZE, const void>
{};

}  // namespace tyl::serialization::binary
