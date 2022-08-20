/**
 * @copyright 2022-present Brian Cairl
 *
 * @file string.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <string>

// Tyl
#include <tyl/serialization/fwd.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/types/common/named.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>std::basic_string<ValueT, CharTraitsT, AllocT></code> save implementation
 */
template <typename OArchive, typename ValueT, typename CharTraitsT, typename AllocT>
struct save<OArchive, std::basic_string<ValueT, CharTraitsT, AllocT>>
{
  void operator()(OArchive& ar, const std::basic_string<ValueT, CharTraitsT, AllocT>& str)
  {
    ar << named{"size", str.size()};
    ar << named{"data", make_packet(str.data(), str.size())};
  }
};

/**
 * @brief Archive-generic <code>std::basic_string<ValueT, CharTraitsT, AllocT></code> load implementation
 */
template <typename IArchive, typename ValueT, typename CharTraitsT, typename AllocT>
struct load<IArchive, std::basic_string<ValueT, CharTraitsT, AllocT>>
{
  void operator()(IArchive& ar, std::basic_string<ValueT, CharTraitsT, AllocT>& str)
  {
    {
      std::size_t size;
      ar >> named{"size", size};
      str.resize(size);
    }

    ar >> named{"data", make_packet(str.data(), str.size())};
  }
};

}  // namespace tyl::serialization
