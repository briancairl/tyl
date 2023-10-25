/**
 * @copyright 2023-present Brian Cairl
 *
 * @file string.hpp
 */
#pragma once

// C++ Standard Library
#include <string>

// Tyl
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename CharT, typename Traits, typename Allocator>
struct save<OArchiveT, std::basic_string<CharT, Traits, Allocator>>
{
  void operator()(OArchiveT& oar, const std::basic_string<CharT, Traits, Allocator>& str)
  {
    oar << named{"len", str.size()};
    oar << named{"data", make_packet(str.data(), str.size())};
  }
};

template <typename IArchiveT, typename CharT, typename Traits, typename Allocator>
struct load<IArchiveT, std::basic_string<CharT, Traits, Allocator>>
{
  void operator()(IArchiveT& iar, std::basic_string<CharT, Traits, Allocator>& str)
  {
    std::size_t len{0};
    iar >> named{"len", len};
    str.resize(len);
    iar >> named{"data", make_packet(str.data(), str.size())};
  }
};

}  // namespace tyl::serialization
