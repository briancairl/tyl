/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_set.hpp
 */
#pragma once

// C++ Standard Library
#include <vector>

// Tyl
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/math.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/std/vector.hpp>

namespace tyl::engine
{

struct TileMap
{
  /// Represents an MxN grid of TileMapSections
  Mat<EntityID, Eigen::Dynamic, Eigen::Dynamic> sections;
};

struct TileMapSection
{
  /// Represents an MxN grid of tiles selected from a TileSet. The size of each tile is dependent on the TileSet it
  /// comes from
  MatXi tile_indices;
};

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct serialize<ArchiveT, engine::TileMap>
{
  void operator()(ArchiveT& ar, engine::TileMap& tile_set) { ar& named{"sections", tile_set.sections}; }
};

template <typename ArchiveT> struct serialize<ArchiveT, engine::TileMapSection>
{
  void operator()(ArchiveT& ar, engine::TileMapSection& tile_set) { ar& named{"tile_indices", tile_set.tile_indices}; }
};

}  // namespace tyl::serialization
