/**
 * @copyright 2023-present Brian Cairl
 *
 * @file text_manager.hpp
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

struct TextAssetManagerOptions
{
  const char* name = "Text Assets";
};

class TextAssetManager
{
public:
  enum class OnCreateErrorCode
  {

  };

  using Options = TextAssetManagerOptions;

  [[nodiscard]] static tyl::expected<TextAssetManager, OnCreateErrorCode> create(const Options& options);

  TextAssetManager(TextAssetManager&&) = default;

  ~TextAssetManager();

  void update(ImGuiContext* const imgui_ctx, entt::registry& reg);

private:
  Options options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TextAssetManager(const Options& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
