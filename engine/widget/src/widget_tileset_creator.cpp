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
namespace
{
using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

struct ImTransform
{
  ImVec2 offset = {0, 0};
  float scaling = 1;
};

ImVec2 truncate(const float scaling, const ImVec2 pt)
{
  const float inv_scaling = 1.f / scaling;
  return {std::floor(pt.x * inv_scaling) * scaling, std::floor(pt.y * inv_scaling) * scaling};
}

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

ImVec2 toImVec2(const Vec2f& v) { return ImVec2{v.x(), v.y()}; }

struct TileSetSelection
{
  Vec2i dims = {10, 10};
  ImVec2 min_corner = {0.f, 0.f};
  dynamic_bitset<std::uint64_t> selected = dynamic_bitset<std::uint64_t>{10 * 10};
};

struct TileSetAtlasTextureEditingState
{
  bool snap_to_pixel = true;
  float zoom_sensivity = 1e-1f;
  ImTransform texture_transform;
  std::optional<ImTransform> texture_transform_on_nav_start = std::nullopt;
};

struct TileSet
{
  Vec2f tile_size = {16, 16};
  std::vector<Rect2f> tiles;
};

void Draw(
  ImDrawList* drawlist,
  const ImTransform& draw_transform,
  const TileSetSelection& tile_selection,
  const ImVec2 tile_size)
{
  static constexpr auto kLineColor = IM_COL32(255, 255, 50, 255);
  static constexpr float kLineThickness = 1;
  if (tile_selection.dims[0] == 1 and tile_selection.dims[1] == 1)
  {
    const auto min_corner = draw_transform * tile_selection.min_corner;
    const auto max_corner = min_corner + tile_size;
    drawlist->AddRect(min_corner, max_corner, kLineColor, kLineThickness);
  }
  else
  {
    const auto min_corner = tile_selection.min_corner;
    const auto max_corner =
      min_corner + ImVec2{tile_selection.dims.x() * tile_size.x, tile_selection.dims.y() * tile_size.y};
    for (int i = 0; i <= tile_selection.dims.x(); ++i)
    {
      const ImVec2 tail{min_corner.x, min_corner.y + tile_size.y * i};
      const ImVec2 head{max_corner.x, min_corner.y + tile_size.y * i};
      drawlist->AddLine(draw_transform * tail, draw_transform * head, kLineColor, kLineThickness);
    }
    for (int i = 0; i <= tile_selection.dims.y(); ++i)
    {
      const ImVec2 tail{min_corner.x + tile_size.x * i, min_corner.y};
      const ImVec2 head{min_corner.x + tile_size.x * i, max_corner.y};
      drawlist->AddLine(draw_transform * tail, draw_transform * head, kLineColor, kLineThickness);
    }
  }
}

}  // namespace

class TileSetCreator::Impl
{
public:
  Impl() {}

  void Browser(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    TileSetPreview(registry, resources);
    TileSetPopUp(registry, resources);
    TileSetNamingPopUp(registry, resources);
  }

  void Creator(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    TileSetAtlasTexture(registry, shared, resources);
  }


  void TileSetPreview(Registry& registry, const WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("#TileSetPreview", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    registry.view<std::string, TileSet, std::vector<TileSetSelection>>().each(
      [this](EntityID id, const auto& label, auto& tileset, const auto& selections) {
        bool is_active = id == active_tile_set_id_;
        ImGui::PushID(static_cast<int>(id));
        ImGui::Checkbox("", &is_active);
        ImGui::SameLine();
        const ImVec2 left_pad{ImGui::GetCursorPos().x, 5};
        if (ImGui::CollapsingHeader(label.c_str()))
        {
          ImGui::Dummy(left_pad);
          ImGui::SameLine();
          ImGui::InputFloat("tile.x", tileset.tile_size.data() + 0);
          ImGui::Dummy(left_pad);
          ImGui::SameLine();
          ImGui::InputFloat("tile.y", tileset.tile_size.data() + 1);
          ImGui::Dummy(left_pad);
          ImGui::SameLine();
          ImGui::TextColored(ImVec4{0.5, 0.5, 0.5, 1.0}, "selections: %lu", selections.size());
          ImGui::Dummy(left_pad);
          ImGui::SameLine();
          ImGui::TextColored(ImVec4{0.5, 0.5, 0.5, 1.0}, "tiles: %lu", tileset.tiles.size());
        }
        ImGui::Separator();
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

  static void TileSetAtlasTextureNav(const ImVec2 mouse_pos, TileSetAtlasTextureEditingState& state)
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
      const float scale_delta = state.zoom_sensivity * delta;
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

  static bool TileSetAtlasEditingSelection(TileSetSelection& selection, const ImVec2 max_corner)
  {
    auto& io = ImGui::GetIO();
    if (const int delta = io.MouseWheel; delta != 0)
    {
      selection.dims.x() = std::max(1, selection.dims.x() + delta);
      selection.dims.y() = std::max(1, selection.dims.y() + delta);
    }
    return !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
  }

  void TileSetAtlasTexture(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild("#TileSetAtlasTexture", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    const auto panel_pos_in_window = ImGui::GetWindowPos();
    const auto mouse_pos_in_panel = ImGui::GetMousePos() - panel_pos_in_window;
    if (active_tile_set_id_)
    {
      auto* drawlist = ImGui::GetWindowDrawList();
      if (registry.any_of<Reference<Texture>>(*active_tile_set_id_))
      {
        auto [texture_ref, tile_set, es, selections] =
          registry.get<Reference<Texture>, TileSet, TileSetAtlasTextureEditingState, std::vector<TileSetSelection>>(
            *active_tile_set_id_);

        const auto& texture = resolve(registry, texture_ref);
        const ImVec2 texture_size{
          static_cast<float>(texture.shape().height), static_cast<float>(texture.shape().width)};

        if (ImGui::IsWindowHovered())
        {
          TileSetAtlasTextureNav(mouse_pos_in_panel, es);
        }

        if (es.snap_to_pixel)
        {
          es.texture_transform.offset = truncate(es.texture_transform.scaling, es.texture_transform.offset);
          if (editting_tile_set_selection_)
          {
            editting_tile_set_selection_->min_corner =
              truncate(es.texture_transform.scaling, editting_tile_set_selection_->min_corner);
          }
        }

        const ImTransform draw_transform = ImTransform{panel_pos_in_window} * es.texture_transform;

        drawlist->AddImage(
          reinterpret_cast<void*>(texture.get_id()), draw_transform * ImVec2{0, 0}, draw_transform * texture_size);

        if (editting_tile_set_selection_)
        {
          if (TileSetAtlasEditingSelection(
                *editting_tile_set_selection_, inverse(draw_transform) * ImGui::GetMousePos()))
          {
            Draw(drawlist, draw_transform, *editting_tile_set_selection_, toImVec2(tile_set.tile_size));
          }
          else
          {
            selections.emplace_back(std::move(editting_tile_set_selection_).value());
            editting_tile_set_selection_.reset();
          }
        }

        for (const auto& selection : selections)
        {
          Draw(drawlist, draw_transform, selection, toImVec2(tile_set.tile_size));
        }
      }
      else
      {
        static constexpr auto kText = "DROP TEXTURE HERE";
        static float kT = 0.0f;
        kT += ImGui::GetIO().DeltaTime;
        drawlist->AddText(
          panel_pos_in_window + (ImGui::GetContentRegionAvail() - ImGui::CalcTextSize(kText)) * 0.5f,
          ImColor{1.0f, 0.2f, 0.5f, std::abs(std::sin(2.f * kT))},
          kText);
      }
    }
    ImGui::EndChild();
    TileSetAtlasTextureSelectionPopUp(registry, resources, mouse_pos_in_panel);
    texture_atlas_is_hovered_ = ImGui::IsItemHovered();
    TileSetAtlasTextureDragAndDropExternalSink(registry, shared, resources);
    TileSetAtlasTextureDragAndDropInternalSink(registry);
  }

  void TileSetAtlasTextureSelectionPopUp(Registry& registry, const WidgetResources& resources, const ImVec2 mouse_pos)
  {
    if (!active_tile_set_id_)
    {
      return;
    }

    static constexpr auto kPopUpName = "#TileSetAtlasTextureSelectionPopUp";
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::BeginPopup(kPopUpName, kPopUpFlags))
    {
      return;
    }

    if (ImGui::BeginMenu("settings"))
    {
      auto& es = registry.get<TileSetAtlasTextureEditingState>(*active_tile_set_id_);
      ImGui::Checkbox("snap to pixel", &es.snap_to_pixel);
      ImGui::SliderFloat("zoom sensitivity", &es.zoom_sensivity, 1e-3f, 5e-1f);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("selection"))
    {
      if (ImGui::MenuItem("create"))
      {
        auto& es = registry.get<TileSetAtlasTextureEditingState>(*active_tile_set_id_);
        editting_tile_set_selection_.emplace();
        editting_tile_set_selection_->min_corner = inverse(es.texture_transform) * mouse_pos;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
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
        editting_tile_set_selection_.reset();
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
        registry.emplace<TileSetAtlasTextureEditingState>(id);
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
        options_.browser_name,
        nullptr,
        (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Browser(registry, shared, resources);
  }
  ImGui::End();

  if (ImGui::Begin(
        options_.creator_name,
        nullptr,
        (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Creator(registry, shared, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine