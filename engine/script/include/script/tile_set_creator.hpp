/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_set_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class TileSetCreator;

struct TileSetCreatorOptions
{
  const char* browser_name = "Tile Set Browser";
  const char* creator_name = "Tile Set Creator";
};

template <> struct ScriptOptions<TileSetCreator>
{
  using type = TileSetCreatorOptions;
};

class TileSetCreator : public ScriptBase<TileSetCreator>
{
  friend class ScriptBase<TileSetCreator>;

public:
  TileSetCreator(TileSetCreator&&) = default;

  ~TileSetCreator();

private:
  static expected<TileSetCreator, ScriptCreationError> CreateImpl(const TileSetCreatorOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  TileSetCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
