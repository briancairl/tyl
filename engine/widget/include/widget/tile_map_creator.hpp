/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_map_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget/widget.hpp>

namespace tyl::engine
{
class TileMapCreator;

struct TileMapCreatorOptions
{
  const char* name = "Tile Map Creator";
};

template <> struct WidgetOptions<TileMapCreator>
{
  using type = TileMapCreatorOptions;
};

class TileMapCreator : public WidgetBase<TileMapCreator>
{
  friend class WidgetBase<TileMapCreator>;

public:
  TileMapCreator(TileMapCreator&&) = default;

  ~TileMapCreator();

private:
  static expected<TileMapCreator, WidgetCreationError> CreateImpl(const TileMapCreatorOptions& options);

  template <typename StreamT> void SaveImpl(WidgetOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(WidgetIArchive<StreamT>& iar);

  WidgetStatus UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources);

  TileMapCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TileMapCreator(const TileMapCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
