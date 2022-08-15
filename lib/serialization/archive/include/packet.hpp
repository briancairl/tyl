/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <type_traits>

namespace tyl::serialization::binary
{
namespace detail
{

template <typename VoidT> struct packet
{
  VoidT* data;
  std::size_t len;
};

template <std::size_t SIZE, typename VoidT> struct packet_fixed_size
{
  VoidT* data;
  static constexpr std::size_t len = SIZE;
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

template <typename PointerT> auto make_packet(PointerT data, std::size_t element_count)
{
  static_assert(std::is_pointer_v<PointerT>, "'PointerT' must be a pointer type");
  using value_type = std::remove_pointer_t<decltype(data)>;
  if constexpr (std::is_const_v<value_type>)
  {
    return const_packet{data, sizeof(value_type) * element_count};
  }
  else
  {
    return packet{data, sizeof(value_type) * element_count};
  }
}

template <typename ObjectT> struct is_packet : std::false_type
{};

template <typename VoidT> struct is_packet<detail::packet<VoidT>> : std::true_type
{};

template <typename ObjectT>
static constexpr bool is_packet_v = is_packet<std::remove_reference_t<std::remove_const_t<ObjectT>>>::value;

}  // namespace tyl::serialization::binary
