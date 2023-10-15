/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
#include <cstring>
#include <memory>
#include <optional>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/async/non_blocking_future.hpp>
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/engine/widgets/tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/utility/dynamic_bitset.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{
namespace
{

constexpr bool kShowBorders = true;
constexpr float kScalingMin = 0.1f;
constexpr float kScalingMax = 10.0f;

}  // namespace

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

  bool LockWindowMovement() { return false; }
};

TilesetCreator::~TilesetCreator() = default;

tyl::expected<TilesetCreator, TilesetCreator::OnCreateErrorCode> TilesetCreator::create(const Options& options)
{
  return TilesetCreator{options, std::make_unique<Impl>()};
}

TilesetCreator::TilesetCreator(const Options& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

void TilesetCreator::update(ImGuiContext* const imgui_ctx, core::Resources& resources)
{
  ImGui::ShowDemoWindow();
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin(options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0)))
  {
    impl_->update(resources);
  }
  ImGui::End();
}

}  // namespace tyl::engine
