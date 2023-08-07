/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// ImGui
#include <imgui.h>

// Tyl
#include <tyl/engine/core/resources_fwd.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::engine::widgets
{

class TilesetCreator
{
public:
  enum class OnCreateErrorCode
  {

  };

  struct Options
  {
    const char* name = "Tile Set Creator";
  };

  [[nodiscard]] static tyl::expected<TilesetCreator, OnCreateErrorCode> create(const Options& options);

  TilesetCreator(TilesetCreator&&) = default;

  ~TilesetCreator();

  void update(ImGuiContext* const imgui_ctx, core::Resources& resources);

private:
  Options options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TilesetCreator(const Options& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
