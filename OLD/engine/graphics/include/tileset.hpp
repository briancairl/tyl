/*
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/math/rect.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl
{

struct Tileset
{
  Vec2f tile_size;
  std::vector<Rect2f> tiles;
};

namespace serialization
{

template <typename IArchive> struct load<IArchive, Tileset>
{
  void operator()(IArchive& ar, Tileset& value)
  {
    ar >> named{"tile_size", make_packet_fixed_size<2>(value.tile_size.data())};
    std::size_t tile_count{0};
    ar >> named{"tile_count", tile_count};
    value.tiles.resize(tile_count);
    ar >> named{"tiles", make_packet(value.tiles.data(), value.tiles.size())};
  }
};

template <typename OArchive> struct save<OArchive, Tileset>
{
  void operator()(OArchive& ar, const Tileset& value)
  {
    ar << named{"tile_size", make_packet_fixed_size<2>(value.tile_size.data())};
    ar << named{"tile_count", value.tiles.size()};
    ar << named{"tiles", make_packet(value.tiles.data(), value.tiles.size())};
  }
};

}  // namespace serialization
}  // namespace tyl
