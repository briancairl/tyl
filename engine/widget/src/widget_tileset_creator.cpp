/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <vector>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/dynamic_bitset.hpp>
#include <tyl/engine/widget_tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/rect.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{

struct Tileset
{
  Reference<tyl::graphics::device::Texture> atlas;
  Vec2f tile_size;
  std::vector<Rect2f> tiles;
};

struct Selection
{
  Vec2i dims = {10, 10};
  Vec2f offset = {0.f, 0.f};
  Vec2f cell_size = {25.f, 25.f};
  dynamic_bitset<std::uint64_t> selected = {};

  Selection(const int rows, const int cols, const Vec2f _cell_size) : dims{rows, cols}, cell_size{_cell_size}
  {
    selected.resize(rows * cols);
  }
};

class TilesetCreator::Impl
{
public:
  Impl() {}

  void Update(Registry& registry, WidgetResources& resources) {}

  bool LockWindowMovement() { return false; }
};

TilesetCreator::~TilesetCreator() = default;

tyl::expected<TilesetCreator, WidgetCreationError> TilesetCreator::CreateImpl(const TilesetCreatorOptions& options)
{
  return TilesetCreator{options, std::make_unique<Impl>()};
}

TilesetCreator::TilesetCreator(const TilesetCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus TilesetCreator::UpdateImpl(Registry& registry, WidgetResources& resources)
{
  if (ImGui::Begin(options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0)))
  {
    impl_->Update(registry, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine