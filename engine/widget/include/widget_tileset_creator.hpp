/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/widget.hpp>

namespace tyl::engine
{
class TileSetCreator;

struct TileSetCreatorOptions
{
  const char* name = "Tile Set Creator";
};

template <> struct WidgetOptions<TileSetCreator>
{
  using type = TileSetCreatorOptions;
};

class TileSetCreator : public Widget<TileSetCreator>
{
  friend class Widget<TileSetCreator>;

public:
  TileSetCreator(TileSetCreator&&) = default;

  ~TileSetCreator();

private:
  static expected<TileSetCreator, WidgetCreationError> CreateImpl(const TileSetCreatorOptions& options);

  WidgetStatus UpdateImpl(Registry& registry, WidgetResources& resources);

  TileSetCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
