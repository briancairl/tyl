/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drag_and_drop.hpp
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

  void update(ImGuiContext* const imgui_ctx, core::Resources& resources);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit DragAndDrop(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::widgets
