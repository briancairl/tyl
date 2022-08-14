/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::serialization::detail
{
namespace detail
{

template <typename VoidT> struct packet
{
  std::size_t len;
  VoidT* data;
};

template <std::size_t SIZE, typename VoidT> struct packet_fixed_size
{
  static constexpr std::size_t len = SIZE;
  VoidT* data;
};

}  // namespace detail

struct packet : detail::packet<void>
{};

struct const_packet : detail::packet<const void>
{};

template <std::size_t SIZE> struct packet_fixed_size : detail::packet_fixed_size<SIZE, void>
{};

template <std::size_t SIZE> struct const_packet_fixed_size : detail::packet_fixed_size<SIZE, const void>
{};

}  // namespace tyl::serialization::detail
