/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.hpp
 */
#pragma once

// Entt
#include <entt/fwd.hpp>

// ImGui
#include <imgui.h>

// Tyl
#include <tyl/utility/expected.hpp>

namespace tyl::engine::widgets
{

class DragAndDrop
{
public:
  enum class OnCreateErrorCode
  {

  };

  struct Options
  {};

  [[nodiscard]] static tyl::expected<DragAndDrop, OnCreateErrorCode> create(const Options& options);

  DragAndDrop(DragAndDrop&&) = default;

  ~DragAndDrop();

  void update(ImGuiContext* const imgui_ctx, entt::registry& reg);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit DragAndDrop(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
