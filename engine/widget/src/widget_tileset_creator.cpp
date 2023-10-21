/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.cpp
 */

#include <iostream>

// C++ Standard Library
#include <memory>
#include <optional>
#include <vector>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/dynamic_bitset.hpp>
#include <tyl/engine/internal/drag_and_drop_images.hpp>
#include <tyl/engine/widget_tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/rect.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{

using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

struct ImTransform
{
  ImVec2 offset = {0, 0};
  float scaling = 1;
};

ImTransform inverse(const ImTransform& transform)
{
  const float inv_scaling = 1.f / transform.scaling;
  return ImTransform{.offset = -transform.offset * transform.scaling, .scaling = inv_scaling};
}

ImVec2 operator*(const ImTransform& transform, const ImVec2 pt) { return pt * transform.scaling + transform.offset; }

ImTransform operator*(const ImTransform& lhs, const ImTransform& rhs)
{
  return ImTransform{
    .offset = {lhs.scaling * rhs.offset.x + lhs.offset.x, lhs.scaling * rhs.offset.y + lhs.offset.y},
    .scaling = (lhs.scaling * rhs.scaling)};
}

struct TileSetSelection
{
  Vec2i dims = {10, 10};
  Vec2f offset = {0.f, 0.f};
  Vec2f cell_size = {25.f, 25.f};
  dynamic_bitset<std::uint64_t> selected = dynamic_bitset<std::uint64_t>{10 * 10};
};

struct TileSetAtlasTextureEdittingState
{
  ImTransform texture_transform;
  std::optional<ImTransform> texture_transform_on_nav_start = std::nullopt;
};

struct TileSet
{
  Vec2f tile_size;
  std::vector<Rect2f> tiles;
};

class TileSetCreator::Impl
{
public:
  Impl() {}

  void Update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    if (ImGui::BeginTable("#TileSetPanels", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
    {
      // Collumn 0
      {
        ImGui::TableNextColumn();
        ImGui::Text("tile sets");
        TileSetPreview(registry, resources);
        TileSetPopUp(registry, resources);
        TileSetNamingPopUp(registry, resources);
      }

      // Collumn 1
      {
        ImGui::TableNextColumn();
        TileSetAtlasTexture(registry, shared, resources);
      }

      ImGui::EndTable();
    }
  }

  void TileSetPreview(Registry& registry, const WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TileSetPreview", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    registry.view<std::string, TileSet>().each([this](EntityID id, const std::string& label, const TileSet& tileset) {
      bool is_active = id == active_tile_set_id_;
      ImGui::PushID(static_cast<int>(id));
      ImGui::Checkbox(label.c_str(), &is_active);
      ImGui::PopID();
      if (is_active)
      {
        active_tile_set_id_ = id;
      }
    });
    ImGui::EndChild();
  }

  void TileSetAtlasTextureDragAndDropInternalSink(Registry& registry)
  {
    if (!ImGui::BeginDragDropTarget())
    {
      return;
    }
    else if (const auto* texture_payload = ImGui::AcceptDragDropPayload("TYL_TEXTURE_ASSET", /*cond = */ 0);
             texture_payload != nullptr)
    {
      TYL_ASSERT_EQ(texture_payload->DataSize, sizeof(EntityID));
      registry.emplace_or_replace<Reference<Texture>>(
        *active_tile_set_id_, *reinterpret_cast<const EntityID*>(texture_payload->Data));
    }
    ImGui::EndDragDropTarget();
  }

  void TileSetAtlasTextureDragAndDropExternalSink(
    Registry& registry,
    WidgetSharedState& shared,
    const WidgetResources& resources)
  {
    if (!active_tile_set_id_ or registry.any_of<Reference<Texture>>(*active_tile_set_id_))
    {
      return;
    }

    // Handle drag/drop
    const auto loaded_texture_ids_or_error = drag_and_drop_images_.update(
      registry, shared, resources, [is_hovered = texture_atlas_is_hovered_] { return is_hovered; });

    // Create reference to first loaded texture
    if (loaded_texture_ids_or_error.has_value() and !loaded_texture_ids_or_error->empty())
    {
      registry.emplace<Reference<Texture>>(*active_tile_set_id_, loaded_texture_ids_or_error->front());
    }
  }

  static void TileSetAtlasTextureNav(const ImVec2 mouse_pos, TileSetAtlasTextureEdittingState& state)
  {
    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl)
    {
      state.texture_transform_on_nav_start.reset();
      return;
    }

    // Zooming
    if (const float delta = io.MouseWheel; delta != 0)
    {
      const float scale_delta = 1e-1f * delta;
      const ImTransform mouse_transform{mouse_pos, 1.f};
      const ImTransform mouse_relative_to_texture = inverse(mouse_transform) * state.texture_transform;
      state.texture_transform =
        mouse_transform * ImTransform{.scaling = (1.f + scale_delta)} * mouse_relative_to_texture;
    }

    // Moving with mouse
    if (!ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonLeft))
    {
      state.texture_transform_on_nav_start.reset();
    }
    else if (state.texture_transform_on_nav_start)
    {
      const auto delta = ImGui::GetMouseDragDelta(ImGuiPopupFlags_MouseButtonLeft);
      state.texture_transform.offset = state.texture_transform_on_nav_start->offset + delta;
    }
    else
    {
      state.texture_transform_on_nav_start = state.texture_transform;
    }
  }

  void TileSetAtlasTexture(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    const auto draw_origin = ImGui::GetCursorScreenPos();

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild("#TileSetAtlasTexture", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    if (active_tile_set_id_)
    {
      auto* drawlist = ImGui::GetWindowDrawList();
      if (registry.any_of<Reference<Texture>>(*active_tile_set_id_))
      {
        auto [texture_ref, editting_state] =
          registry.get<Reference<Texture>, TileSetAtlasTextureEdittingState>(*active_tile_set_id_);

        const auto& texture = resolve(registry, texture_ref);
        const ImVec2 texture_size{
          static_cast<float>(texture.shape().height), static_cast<float>(texture.shape().width)};

        if (ImGui::IsWindowHovered())
        {
          TileSetAtlasTextureNav(ImGui::GetMousePos() - draw_origin, editting_state);
        }

        drawlist->AddImage(
          reinterpret_cast<void*>(texture.get_id()),
          draw_origin + editting_state.texture_transform * ImVec2{0, 0},
          draw_origin + editting_state.texture_transform * texture_size,
          ImVec2(0, 0),
          ImVec2(1, 1));
      }
      else
      {
        drawlist->AddText(draw_origin + ImVec2{50, 50}, ImColor{1.0f, 0.5f, 0.5f, 1.0f}, "DROP TEXTURE HERE");
      }
    }
    ImGui::EndChild();
    texture_atlas_is_hovered_ = ImGui::IsItemHovered();
    TileSetAtlasTextureDragAndDropExternalSink(registry, shared, resources);
    TileSetAtlasTextureDragAndDropInternalSink(registry);
  }

  void TileSetPopUp(Registry& registry, const WidgetResources& resources)
  {
    static constexpr auto kPopUpName = "#TileSetPopUp";
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::BeginPopup(kPopUpName, kPopUpFlags))
    {
      return;
    }

    if (ImGui::BeginMenu("edit"))
    {
      if (ImGui::MenuItem("create"))
      {
        tile_set_naming_pop_up_open_ = true;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("delete") and active_tile_set_id_)
      {
        registry.destroy(*active_tile_set_id_);
        active_tile_set_id_.reset();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }

  void TileSetNamingPopUp(Registry& registry, const WidgetResources& resources)
  {
    static constexpr auto kPopUpName = "#TileSetNamingPopUp";
    if (tile_set_naming_pop_up_open_ and !ImGui::IsPopupOpen(kPopUpName))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_NoTitleBar;
    if (!ImGui::BeginPopupModal(kPopUpName, &tile_set_naming_pop_up_open_, kPopUpFlags))
    {
      return;
    }

    static constexpr auto kTileSetNameFlags = ImGuiInputTextFlags_EnterReturnsTrue;
    static constexpr const char* kTileSetNameBufferDefault = "new tileset";
    static char kTileSetNameBuffer[100] = "new tileset";
    if (ImGui::InputText("name", kTileSetNameBuffer, sizeof(kTileSetNameBuffer), kTileSetNameFlags))
    {
      {
        const auto id = registry.create();
        registry.emplace<std::string>(id, kTileSetNameBuffer);
        registry.emplace<TileSet>(id);
        registry.emplace<TileSetAtlasTextureEdittingState>(id);
        registry.emplace<std::vector<TileSetSelection>>(id, std::vector<TileSetSelection>{});
        active_tile_set_id_ = id;
      }
      std::strcpy(kTileSetNameBuffer, kTileSetNameBufferDefault);
      tile_set_naming_pop_up_open_ = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  constexpr bool LockWindowMovement() const { return texture_atlas_is_hovered_; }

private:
  bool texture_atlas_is_hovered_ = false;
  bool tile_set_naming_pop_up_open_ = false;
  std::optional<EntityID> active_tile_set_id_;
  std::optional<TileSetSelection> editting_tile_set_selection_;
  DragAndDropImages drag_and_drop_images_;
};

TileSetCreator::~TileSetCreator() = default;

tyl::expected<TileSetCreator, WidgetCreationError> TileSetCreator::CreateImpl(const TileSetCreatorOptions& options)
{
  return TileSetCreator{options, std::make_unique<Impl>()};
}

TileSetCreator::TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

WidgetStatus TileSetCreator::UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
  if (ImGui::Begin(
        options_.name, nullptr, (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Update(registry, shared, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine