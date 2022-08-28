/**
 * @copyright 2022-present Brian Cairl
 *
 * @file array.hpp
 */
#pragma once

// C++ Standard Library
#include <array>
#include <cstdint>

// Tyl
#include <tyl/serialization/fwd.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>std::array<ValueT, N></code> save implementation
 */
template <typename OArchive, typename ValueT, std::size_t N> struct save<OArchive, std::array<ValueT, N>>
{
  void operator()(OArchive& ar, const std::array<ValueT, N>& arr)
  {
    if constexpr (is_trivially_serializable_v<OArchive, ValueT>)
    {
      ar << named{"data", make_packet(arr.data(), arr.size())};
    }
    else
    {
      for (const auto& element : arr)
      {
        ar << element;
      }
    }
  }
};

/**
 * @brief Archive-generic <code>std::array<ValueT, N></code> load implementation
 */
template <typename IArchive, typename ValueT, std::size_t N> struct load<IArchive, std::array<ValueT, N>>
{
  void operator()(IArchive& ar, std::array<ValueT, N>& arr)
  {
    if constexpr (is_trivially_serializable_v<IArchive, ValueT>)
    {
      ar >> named{"data", make_packet(arr.data(), arr.size())};
    }
    else
    {
      for (auto& element : arr)
      {
        ar >> element;
      }
    }
  }
};

}  // namespace tyl::serialization
