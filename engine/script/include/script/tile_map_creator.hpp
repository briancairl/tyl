/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_map_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class TileMapCreator;

struct TileMapCreatorOptions
{
  const char* name = "Tile Map Creator";
};

template <> struct ScriptOptions<TileMapCreator>
{
  using type = TileMapCreatorOptions;
};

class TileMapCreator : public ScriptBase<TileMapCreator>
{
  friend class ScriptBase<TileMapCreator>;

public:
  TileMapCreator(TileMapCreator&&) = default;

  ~TileMapCreator();

private:
  static expected<TileMapCreator, ScriptCreationError> CreateImpl(const TileMapCreatorOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  TileMapCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TileMapCreator(const TileMapCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
