/**
 * @copyright 2023-present Brian Cairl
 *
 * @file vector.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename ValueT, typename Allocator>
struct save<OArchiveT, std::vector<ValueT, Allocator>>
{
  void operator()(OArchiveT& oar, const std::vector<ValueT, Allocator>& vec)
  {
    oar << named{"len", vec.size()};
    if constexpr (is_trivially_serializable_v<OArchiveT, ValueT>)
    {
      oar << named{"data", make_packet(vec.data(), vec.size())};
    }
    else
    {
      oar << named{"data", make_sequence(vec.begin(), vec.end())};
    }
  }
};

template <typename IArchiveT, typename ValueT, typename Allocator>
struct load<IArchiveT, std::vector<ValueT, Allocator>>
{
  void operator()(IArchiveT& iar, std::vector<ValueT, Allocator>& vec)
  {
    std::size_t len{0};
    iar >> named{"len", len};
    vec.resize(len);
    if constexpr (is_trivially_serializable_v<IArchiveT, ValueT>)
    {
      iar >> named{"data", make_packet(vec.data(), vec.size())};
    }
    else
    {
      iar >> named{"data", make_sequence(vec.begin(), vec.end())};
    }
  }
};

}  // namespace tyl::serialization
