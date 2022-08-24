/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <type_traits>

namespace tyl::serialization
{

template <typename PointerT> struct basic_packet
{
  static_assert(std::is_pointer_v<PointerT>);
  PointerT data;
  std::size_t len;
};

template <typename PointerT, std::size_t Len> struct basic_packet_fixed_size
{
  static_assert(std::is_pointer_v<PointerT>);
  PointerT data;
  static constexpr std::size_t len = Len;
};

struct packet : basic_packet<void*>
{};

struct const_packet : basic_packet<const void*>
{};

template <std::size_t Len> struct packet_fixed_size : basic_packet_fixed_size<void*, Len>
{};

template <std::size_t Len> struct const_packet_fixed_size : basic_packet_fixed_size<const void*, Len>
{};

template <typename PointerT> auto make_packet(PointerT data, std::size_t element_count)
{
  static_assert(std::is_pointer_v<PointerT>, "'PointerT' must be a pointer type");

  using value_type = std::remove_pointer_t<PointerT>;

  if constexpr (std::is_const_v<value_type>)
  {
    return const_packet{data, sizeof(value_type) * element_count};
  }
  else
  {
    return packet{data, sizeof(value_type) * element_count};
  }
}

template <typename PointerT> auto make_packet(PointerT data)
{
  static_assert(std::is_pointer_v<PointerT>, "'PointerT' must be a pointer type");

  using value_type = std::remove_pointer_t<PointerT>;

  static_assert(!std::is_void_v<value_type>, "'PointerT' must not be a void pointer");

  if constexpr (std::is_const_v<value_type>)
  {
    return const_packet_fixed_size<sizeof(value_type)>{data};
  }
  else
  {
    return packet_fixed_size<sizeof(value_type)>{data};
  }
}

template <typename T> struct is_packet : std::false_type
{};

template <> struct is_packet<packet> : std::true_type
{};

template <> struct is_packet<const_packet> : std::true_type
{};

template <std::size_t Len> struct is_packet<packet_fixed_size<Len>> : std::true_type
{};

template <std::size_t Len> struct is_packet<const_packet_fixed_size<Len>> : std::true_type
{};

template <typename T>
static constexpr bool is_packet_v = is_packet<std::remove_const_t<std::remove_reference_t<T>>>::value;

}  // namespace tyl::serialization
