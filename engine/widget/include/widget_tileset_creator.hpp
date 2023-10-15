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
class TilesetCreator;

struct TilesetCreatorOptions
{
  const char* name = "Tile Set Creator";
};

template <> struct WidgetOptions<TilesetCreator>
{
  using type = TilesetCreatorOptions;
};

class TilesetCreator : public Widget<TilesetCreator>
{
  friend class Widget<TilesetCreator>;

public:
  TilesetCreator(TilesetCreator&&) = default;

  ~TilesetCreator();

private:
  static expected<TilesetCreator, WidgetCreationError> CreateImpl(const TilesetCreatorOptions& options);

  WidgetStatus UpdateImpl(Registry& registry, WidgetResources& resources);

  TilesetCreatorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TilesetCreator(const TilesetCreatorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
