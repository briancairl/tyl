/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Entt
#include <entt/fwd.hpp>

// ImGui
#include <imgui.h>

// Tyl
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
  {};

  [[nodiscard]] static tyl::expected<TilesetCreator, OnCreateErrorCode> create(const Options& options);

  TilesetCreator(TilesetCreator&&) = default;

  ~TilesetCreator();

  void update(ImGuiContext* const imgui_ctx, entt::registry& reg);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit TilesetCreator(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
