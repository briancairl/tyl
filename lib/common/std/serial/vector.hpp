/**
 * @copyright 2022-present Brian Cairl
 *
 * @file vector.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <vector>

// Tyl
#include <tyl/serialization/fwd.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>std::vector<ValueT, AllocT></code> save implementation
 */
template <typename OArchive, typename ValueT, typename AllocT> struct save<OArchive, std::vector<ValueT, AllocT>>
{
  void operator()(OArchive& ar, const std::vector<ValueT, AllocT>& vec)
  {
    ar << named{"size", vec.size()};
    if constexpr (is_trivially_serializable_v<OArchive, ValueT>)
    {
      ar << named{"data", make_packet(vec.data(), vec.size())};
    }
    else
    {
      ar << named{"data", make_sequence(vec.begin(), vec.end())};
    }
  }
};

/**
 * @brief Archive-generic <code>std::vector<ValueT, AllocT></code> load implementation
 */
template <typename IArchive, typename ValueT, typename AllocT> struct load<IArchive, std::vector<ValueT, AllocT>>
{
  void operator()(IArchive& ar, std::vector<ValueT, AllocT>& vec)
  {
    {
      std::size_t size;
      ar >> named{"size", size};
      vec.resize(size);
    }

    if constexpr (is_trivially_serializable_v<IArchive, ValueT>)
    {
      ar >> named{"data", make_packet(vec.data(), vec.size())};
    }
    else
    {
      ar >> named{"data", make_sequence(vec.begin(), vec.end())};
    }
  }
};

}  // namespace tyl::serialization
