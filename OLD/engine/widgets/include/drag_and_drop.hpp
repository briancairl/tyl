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
#include <tyl/engine/widgets/widget.hpp>

namespace tyl::engine
{

class DragAndDrop : public Widget<DragAndDrop>
{
  friend class Widget<DragAndDrop>;

public:
  struct Options
  {};

  DragAndDrop(DragAndDrop&&) = default;
  ~DragAndDrop();

private:
  WidgetStatus UpdateImpl(Registry& registry, WidgetResources& resources);
  static expected<DragAndDrop, WidgetCreationError> CreateImpl(const Options& options);

  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit DragAndDrop(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
