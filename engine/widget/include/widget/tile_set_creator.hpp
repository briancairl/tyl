/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tile_set_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget/widget.hpp>

namespace tyl::engine
{
class TileSetCreator;

struct TileSetCreatorOptions
{
  const char* browser_name = "Tile Set Browser";
  const char* creator_name = "Tile Set Creator";
};

template <> struct WidgetOptions<TileSetCreator>
{
  using type = TileSetCreatorOptions;
};

class TileSetCreator : public WidgetBase<TileSetCreator>
{
  friend class WidgetBase<TileSetCreator>;

public:
  TileSetCreator(TileSetCreator&&) = default;

  ~TileSetCreator();

private:
  static expected<TileSetCreator, WidgetCreationError> CreateImpl(const TileSetCreatorOptions& options);

  template <typename StreamT> void SaveImpl(WidgetOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(WidgetIArchive<StreamT>& iar);

  WidgetStatus UpdateImpl(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources);

  TileSetCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
