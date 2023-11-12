/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_set.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/engine/math.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/std/vector.hpp>

namespace tyl::engine
{

struct TileSet
{
  /// Size of all tiles in pixels
  Vec2f tile_size = {16, 16};
  /// Rectangles defining the location of each tile in UV-space of an atlas texture
  std::vector<Rect2f> tiles = {};
};

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct serialize<ArchiveT, engine::TileSet>
{
  void operator()(ArchiveT& ar, engine::TileSet& tile_set)
  {
    ar& named{"tile_size", tile_set.tile_size};
    ar& named{"tiles", tile_set.tiles};
  }
};

}  // namespace tyl::serialization
