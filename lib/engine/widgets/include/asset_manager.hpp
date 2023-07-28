/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <memory>
#include <vector>

// Entt
#include <entt/fwd.hpp>

// ImGui
#include <imgui.h>

// Tyl
#include <tyl/utility/expected.hpp>

namespace tyl::engine::widgets
{

class AssetManager
{
public:
  enum class OnCreateErrorCode
  {

  };

  struct Options
  {};

  [[nodiscard]] static tyl::expected<AssetManager, OnCreateErrorCode> create(const Options& options);

  AssetManager(AssetManager&&) = default;

  ~AssetManager();

  void update(ImGuiContext* const imgui_ctx, entt::registry& reg);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit AssetManager(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
