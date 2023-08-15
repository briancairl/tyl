/**
 * @copyright 2023-present Brian Cairl
 *
 * @file texture_manager.hpp
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

struct TextureAssetManagerOptions
{
  const char* name = "Texture Assets";
};

class TextureAssetManager
{
public:
  enum class OnCreateErrorCode
  {

  };

  using Options = TextureAssetManagerOptions;

  [[nodiscard]] static tyl::expected<TextureAssetManager, OnCreateErrorCode> create(const Options& options);

  TextureAssetManager(TextureAssetManager&&) = default;

  ~TextureAssetManager();

  void update(ImGuiContext* const imgui_ctx, core::Resources& resources);

private:
  Options options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  TextureAssetManager(const Options& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
