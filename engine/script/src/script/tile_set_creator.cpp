/**
 * @copyright 2023-present Brian Cairl
 *
 * @file script_tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <optional>
#include <vector>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/asset.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/internal/imgui_widgets.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/tile_set_creator.hpp>
#include <tyl/engine/tile_set.hpp>
#include <tyl/format.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/rect.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/named_ignored.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/std/optional.hpp>
#include <tyl/serialization/std/string.hpp>
#include <tyl/serialization/std/vector.hpp>

// 1. Create new named tileset
// 2. Select texture
// 3. Select grids

namespace tyl::engine
{
namespace
{
using Image = graphics::host::Image;
using Texture = graphics::device::Texture;

struct TileSetSelection
{
  int rows = 10;
  int cols = 10;
  ImVec2 pos = {0.f, 0.f};

  ImColor grid_color = IM_COL32_WHITE;
  float grid_line_thickness = 1.f;
};

using TileSetEditingState = std::vector<Reference<TileSetSelection>>;

struct AtlasTextureEditingState
{
  bool show_grid = true;
  bool show_border = false;
  bool show_source_filename = false;
  bool show_position = true;
  float zoom_sensivity = 1e-1f;
  ImColor texture_tint = ImColor{1.f, 1.f, 1.f, 1.f};
  ImTransform window_to_texture;
  std::optional<ImTransform> window_to_texture_on_nav_start = std::nullopt;
};

ImVec2 DrawGrid(
  ImDrawList* drawlist,
  const ImVec2 pos,
  const ImVec2 step,
  const int rows,
  const int cols,
  const ImU32 color,
  const float thickness = 1.f)
{
  if (rows == 1 and cols == 1)
  {
    const auto top = pos + step;
    drawlist->AddRect(pos, top, color, 0.5 * thickness, ImDrawFlags_None, thickness);
    return top;
  }
  else
  {
    const auto top = pos + ImVec2{cols * step.x, rows * step.y};
    for (int i = 1; i < rows; ++i)
    {
      const ImVec2 tail{pos.x, pos.y + step.y * i};
      const ImVec2 head{top.x, pos.y + step.y * i};
      drawlist->AddLine(tail, head, color, thickness);
    }
    for (int i = 1; i < cols; ++i)
    {
      const ImVec2 tail{pos.x + step.x * i, pos.y};
      const ImVec2 head{pos.x + step.x * i, top.y};
      drawlist->AddLine(tail, head, color, thickness);
    }
    drawlist->AddRect(pos, top, color, 0.5 * thickness, ImDrawFlags_None, thickness);
    return top;
  }
}

void ImTileSmallPreview(const TileSet& tile_set, const Texture& texture, ImVec2 size)
{
  const auto avail = ImGui::GetContentRegionAvail();
  const auto pos = ImGui::GetCursorScreenPos();
  size.x = (size.x == 0) ? avail.x : size.x;
  size.y = (size.y == 0) ? avail.y : size.y;

  const float limiting_dimension = std::min(size.x, size.y);
  const float tile_aspect_ratio = tile_set.tile_size.x() / tile_set.tile_size.y();

  const ImVec2 shown_tile_size{0.75f * limiting_dimension * tile_aspect_ratio, 0.75f * limiting_dimension};
  const float v_pad = 0.5f * (size.y - shown_tile_size.y);
  const float h_pad = 0.1f * shown_tile_size.x;

  auto* drawlist = ImGui::GetWindowDrawList();
  ImGui::Dummy(size);

  ImVec2 shift = {v_pad, v_pad};
  const int total_shown = std::floor(size.x / (h_pad + shown_tile_size.x));
  int remaining = total_shown;
  for (const auto& rect : tile_set.tiles)
  {
    const auto min_pt = pos + shift;
    const auto max_pt = min_pt + shown_tile_size;
    const float e = 0.5 + 0.5f * static_cast<float>(remaining) / total_shown;
    drawlist->AddImage(
      reinterpret_cast<void*>(texture.get_id()),
      min_pt,
      max_pt,
      ToImVec2(rect.min()),
      ToImVec2(rect.max()),
      ImColor{e, e, e, e});
    shift.x += (h_pad + shown_tile_size.x);
    if (--remaining == total_shown)
    {
      break;
    }
  }

  drawlist->AddRect(pos, pos + size, ImColor{ImGui::GetStyle().Colors[ImGuiCol_Border]});
}

}  // namespace

using namespace tyl::serialization;

class TileSetCreator::Impl
{
public:
  Impl() :
      tile_set_naming_pop_up_{"new tile set name", "new tile set name", 400.F},
      tile_set_rename_pop_up_{"tile set name", "rename tile set", 400.F},
      tile_set_delete_confirmation_{"delete selected tile set?", 250.F},
      tile_set_size_submission_pop_up_{"update tile size", 250.F}
  {}

  void Browser(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    TileSetPreview(scene, resources);
    TileSetCreateMenu();
    tile_set_naming_pop_up_.update([this](const char* name) {
      const auto id = local_registry_.create();
      local_registry_.emplace<std::string>(id, name);
      local_registry_.emplace<TileSet>(id, Vec2f{16, 16});
      local_registry_.emplace<AtlasTextureEditingState>(id);
      local_registry_.emplace<TileSetEditingState>(id);
      local_registry_.emplace<Reference<Texture>>(id);
      editing_tile_set_id_ = id;
    });
  }

  void Creator(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    time_elapsed_seconds_ += ImGui::GetIO().DeltaTime;
    time_elapsed_fadeosc_ = 0.5f + 0.5 * std::sin(2.f * time_elapsed_seconds_);
    AtlasTexturePreview(scene, shared, resources);
    TileSetSubmitSelections(scene);
  }

  void TileSetSubmitSelections(const Scene& scene)
  {
    if (!texture_atlas_is_hovered_ or !editing_tile_set_id_ or !ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      return;
    }

    const auto [atlas_texture_ref, tile_set, tile_set_selections] =
      local_registry_.get<Reference<Texture>, TileSet, TileSetEditingState>(*editing_tile_set_id_);

    if ((atlas_texture_ref == nullptr) or tile_set_selections.empty())
    {
      return;
    }

    tile_set.tiles.clear();

    const auto& atlas_texture = resolve(scene.assets, atlas_texture_ref);
    for (const auto& selection_ref : tile_set_selections)
    {
      const auto& selection = resolve(local_registry_, selection_ref);
      const auto tile_size = ToImVec2(tile_set.tile_size);
      for (int i = 0; i < selection.cols; ++i)
      {
        for (int j = 0; j < selection.rows; ++j)
        {
          const ImVec2 min_pt = selection.pos + ImVec2{i * tile_size.x, j * tile_size.y};
          const ImVec2 max_pt = min_pt + tile_size;
          const ImVec2 min_pt_uv{min_pt.x / atlas_texture.shape().height, min_pt.y / atlas_texture.shape().width};
          const ImVec2 max_pt_uv{max_pt.x / atlas_texture.shape().height, max_pt.y / atlas_texture.shape().width};
          tile_set.tiles.emplace_back(FromImVec2(min_pt_uv), FromImVec2(max_pt_uv));
        }
      }
    }
  }

  void TileSetPreview(Scene& scene, const ScriptResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("##TileSetPreview", ImVec2{0, 200}, kChildShowBoarders, kChildFlags);
    if (ImGui::BeginTable("##TileSetPreviewTable", 3, ImGuiTableFlags_Resizable))
    {
      ImGui::TableSetupColumn("name");
      ImGui::TableSetupColumn("tile size");
      ImGui::TableSetupColumn("tile count");
      ImGui::TableHeadersRow();

      local_registry_.view<std::string, TileSet>().each([&](EntityID id, auto& label, auto& tile_set) {
        const auto atlas_texture_ref = local_registry_.get<Reference<Texture>>(id);

        if (ImGui::TableNextColumn())
        {
          ImGui::PushID(static_cast<int>(id));
          ImGui::Text("%s", label.c_str());
          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
            editing_tile_set_id_ = id;
          }

          if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
          {
            editing_tile_set_id_ = id;
            tile_set_rename_pop_up_.open(label);
          }

          if (editing_tile_set_id_ == id and tile_set_rename_pop_up_.is_open())
          {
            tile_set_rename_pop_up_.update([&label](const char* name) { label = name; });
          }

          ImGui::PopID();
        }

        if (ImGui::TableNextColumn())
        {
          ImGui::Text("(%.1f x %.1f)", tile_set.tile_size.x(), tile_set.tile_size.y());
          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
            editing_tile_set_id_ = id;
          }

          if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
          {
            editing_tile_set_id_ = id;
            tile_set_size_submission_pop_up_.open();
          }

          if (
            (editing_tile_set_id_ == id) and
            tile_set_size_submission_pop_up_.is_submitted(
              [&tile_set, width = tile_set_size_submission_pop_up_.width_internal()] {
                static constexpr auto kInputFloatFlags = ImGuiInputTextFlags_EnterReturnsTrue;
                ImGui::SetNextItemWidth(width);
                return ImGui::InputFloat2("##tile_size", tile_set.tile_size.data(), "%.1f", kInputFloatFlags);
              }))
          {
            TileSetSubmitSelections(scene);
          }
        }

        if (editing_tile_set_id_ == id)
        {
          ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 1.f, 0.f, 0.25f});
        }

        if (ImGui::TableNextColumn())
        {
          ImGui::SeparatorText(format("tiles: %lu", tile_set.tiles.size()));
          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
            editing_tile_set_id_ = id;
          }
          if (atlas_texture_ref == nullptr)
          {
            return;
          }
          else if (auto* atlas_texture = maybe_resolve(scene.assets, atlas_texture_ref); atlas_texture != nullptr)
          {
            ImTileSmallPreview(tile_set, *atlas_texture, ImVec2{0, 50});
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
              editing_tile_set_id_ = id;
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
              ImGui::Text("%s", label.c_str());
              ImTileSmallPreview(tile_set, *atlas_texture, ImVec2{100, 25});
              if (ImGui::SetDragDropPayload("TYL_TILE_SET_ASSET", std::addressof(id), sizeof(EntityID), /*cond = */ 0))
              {}
              ImGui::EndDragDropSource();
            }
          }
        }
      });
      ImGui::EndTable();
    }
    ImGui::EndChild();
  }

  void AtlasTextureDragAndDropInternalSink()
  {
    if (!ImGui::BeginDragDropTarget())
    {
      return;
    }
    else if (const auto* texture_payload = ImGui::AcceptDragDropPayload("TYL_TEXTURE_ASSET", /*cond = */ 0);
             texture_payload != nullptr)
    {
      TYL_ASSERT_EQ(texture_payload->DataSize, sizeof(EntityID));
      local_registry_.emplace_or_replace<Reference<Texture>>(
        *editing_tile_set_id_, *reinterpret_cast<const EntityID*>(texture_payload->Data));
    }
    ImGui::EndDragDropTarget();
  }

  static void AtlasTextureNav(const ImTransform& view_to_pointer, AtlasTextureEditingState& state)
  {
    auto& GuiIO = ImGui::GetIO();
    if (!GuiIO.KeyCtrl)
    {
      state.window_to_texture_on_nav_start.reset();
      return;
    }

    // Zooming
    if (const float delta = GuiIO.MouseWheel; delta != 0)
    {
      const ImTransform pointer_to_texture = ImInverse(view_to_pointer) * state.window_to_texture;
      const ImTransform pointer_to_scaled{.scaling = (1.f + state.zoom_sensivity * delta)};
      state.window_to_texture = view_to_pointer * pointer_to_scaled * pointer_to_texture;
    }

    // Moving with mouse
    if (!ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonLeft))
    {
      state.window_to_texture_on_nav_start.reset();
    }
    else if (state.window_to_texture_on_nav_start)
    {
      const auto delta = ImGui::GetMouseDragDelta(ImGuiPopupFlags_MouseButtonLeft);
      state.window_to_texture.offset = state.window_to_texture_on_nav_start->offset + delta;
    }
    else
    {
      state.window_to_texture_on_nav_start = state.window_to_texture;
    }
  }

  void AtlasTexturePreview(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    if (!editing_tile_set_id_)
    {
      return;
    }

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild("#AtlasTexture", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);

    auto* drawlist = ImGui::GetWindowDrawList();
    auto [editing_state, atlas_texture_ref] =
      local_registry_.get<AtlasTextureEditingState, Reference<Texture>>(*editing_tile_set_id_);

    const ImTransform screen_to_window{ImGui::GetWindowPos()};
    const ImTransform screen_to_pointer{ImGui::GetMousePos()};
    const ImTransform screen_to_texture = screen_to_window * editing_state.window_to_texture;

    if (ImGui::IsWindowHovered())
    {
      const auto view_to_pointer = ImInverse(screen_to_window) * screen_to_pointer;
      AtlasTextureNav(view_to_pointer, editing_state);
    }

    if (atlas_texture_ref == nullptr)
    {
      const auto& label = local_registry_.get<std::string>(*editing_tile_set_id_);
      const auto* text = format("DROP TEXTURE HERE FOR [%s]", label.c_str());
      drawlist->AddText(
        screen_to_window.offset + (ImGui::GetContentRegionAvail() - ImGui::CalcTextSize(text)) * 0.5f,
        ImColor{ImFadeColor(ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget], time_elapsed_fadeosc_)},
        text);
    }
    else if (is_valid(scene.assets, atlas_texture_ref))
    {
      const auto& texture = resolve(scene.assets, atlas_texture_ref);
      const ImVec2 texture_size{static_cast<float>(texture.shape().height), static_cast<float>(texture.shape().width)};
      const ImVec2 texture_min_corner = screen_to_texture * ImVec2{0, 0};
      const ImVec2 texture_max_corner = screen_to_texture * texture_size;

      drawlist->AddImage(
        reinterpret_cast<void*>(texture.get_id()),
        texture_min_corner,
        texture_max_corner,
        ImVec2{0, 0},
        ImVec2{1, 1},
        editing_state.texture_tint);

      if (editing_state.show_grid)
      {
        auto& tile_set = local_registry_.get<TileSet>(*editing_tile_set_id_);
        const int rows = texture.shape().width / tile_set.tile_size.y();
        const int cols = texture.shape().height / tile_set.tile_size.x();
        DrawGrid(
          drawlist,
          texture_min_corner,
          screen_to_texture ^ ToImVec2(tile_set.tile_size),
          rows,
          cols,
          ImColor{ImGui::GetStyle().Colors[ImGuiCol_Border]});
        drawlist->AddRect(texture_min_corner, texture_max_corner, ImColor{ImGui::GetStyle().Colors[ImGuiCol_Border]});
      }
      else if (editing_state.show_border)
      {
        drawlist->AddRect(texture_min_corner, texture_max_corner, ImColor{ImGui::GetStyle().Colors[ImGuiCol_Border]});
      }

      if (editing_state.show_source_filename)
      {
        const auto& asset_location = scene.assets.get<AssetLocation<Texture>>(*atlas_texture_ref.id);
        drawlist->AddText(
          texture_min_corner + ImVec2{0, -20},
          ImColor{ImGui::GetStyle().Colors[ImGuiCol_Text]},
          asset_location.path.string().c_str());
      }

      AtlasTextureEditSelection(drawlist, screen_to_texture);
    }
    else
    {
      const auto& label = local_registry_.get<std::string>(*editing_tile_set_id_);
      const auto* text = format<128>("TEXTURE FOR [%s] IS INVALID! PERHAPS UNLOADED OR MISSING?", label.c_str());
      drawlist->AddText(
        screen_to_window.offset + (ImGui::GetContentRegionAvail() - ImGui::CalcTextSize(text)) * 0.5f,
        ImColor{ImFadeColor(ImVec4{1, 0, 0, 1}, time_elapsed_fadeosc_)},
        text);
    }

    ImGui::EndChild();
    texture_atlas_is_hovered_ = ImGui::IsItemHovered();

    const auto texture_to_screen = ImInverse(screen_to_texture);
    const auto texture_to_pointer = texture_to_screen * screen_to_pointer;

    if (editing_state.show_position)
    {
      const auto* text = format(
        "(%f, %f) [%.3f%%]",
        texture_to_pointer.offset.x,
        texture_to_pointer.offset.y,
        100.f / texture_to_pointer.scaling);
      drawlist->AddText(screen_to_pointer.offset, ImColor{ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]}, text);
    }

    AtlasTexturePopUp(texture_to_screen);
    AtlasTextureDragAndDropInternalSink();
  }

  void AtlasTexturePopUp(const ImTransform& texture_to_screen)
  {
    static constexpr auto kPopUpName = "#AtlasTexturePopUp";
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup(kPopUpName);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::BeginPopup(kPopUpName, kPopUpFlags))
    {
      return;
    }

    const ImVec2 screen_popup_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
    const bool atlas_texture_is_set = (local_registry_.get<Reference<Texture>>(*editing_tile_set_id_) != nullptr);
    const bool selection_is_active = static_cast<bool>(editing_tile_set_selection_id_);

    if (ImGui::BeginMenu("settings", atlas_texture_is_set))
    {
      auto& editing_state = local_registry_.get<AtlasTextureEditingState>(*editing_tile_set_id_);
      ImGui::Checkbox("show grid", &editing_state.show_grid);
      ImGui::Checkbox("show border", &editing_state.show_border);
      ImGui::Checkbox("show source", &editing_state.show_source_filename);
      ImGui::Checkbox("show position", &editing_state.show_position);
      if (ImGui::BeginMenu("zoom sensitivity", atlas_texture_is_set))
      {
        ImGui::SliderFloat("##zoom_sensitivity", &editing_state.zoom_sensivity, 1e-3f, 5e-1f);
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("edit texture tint", atlas_texture_is_set))
      {
        ImGui::ColorPicker4("##texture_tint", reinterpret_cast<float*>(&editing_state.texture_tint.Value));
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("create", atlas_texture_is_set))
    {
      if (ImGui::MenuItem("new", nullptr, false, true))
      {
        // Create new selection
        const auto id = local_registry_.create();
        local_registry_.emplace<TileSetSelection>(id).pos = ImTruncate(texture_to_screen * screen_popup_pos);
        // Add to current tileset
        local_registry_.get<TileSetEditingState>(*editing_tile_set_id_).emplace_back(Reference<TileSetSelection>{id});
        // Set active selection
        editing_tile_set_selection_id_ = id;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("duplicate", nullptr, false, selection_is_active))
      {
        // Get selection we want to duplicate
        const auto& selection_to_copy = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
        // Create new selection
        const auto id = local_registry_.create();
        local_registry_.emplace<TileSetSelection>(id, selection_to_copy).pos = texture_to_screen * screen_popup_pos;
        // Add to current tileset
        local_registry_.get<TileSetEditingState>(*editing_tile_set_id_).emplace_back(Reference<TileSetSelection>{id});
        // Set active selection
        editing_tile_set_selection_id_ = id;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("selection", atlas_texture_is_set))
    {
      if (ImGui::MenuItem("deselect", nullptr, false, selection_is_active))
      {
        // Reset active selection
        editing_tile_set_selection_id_.reset();
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("move to", nullptr, false, selection_is_active))
      {
        // Set selection position to popup top-left corner
        auto& selection = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
        selection.pos = texture_to_screen * screen_popup_pos;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::BeginMenu("properties", selection_is_active))
      {
        auto& selection = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
        ImGui::InputFloat2("position", reinterpret_cast<float*>(&selection.pos));
        if (ImGui::InputInt("rows", &selection.rows))
          selection.rows = std::max(selection.rows, 1);
        if (ImGui::InputInt("cols", &selection.cols))
          selection.cols = std::max(selection.cols, 1);
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("view", selection_is_active))
      {
        if (ImGui::BeginMenu("color"))
        {
          auto& selection = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
          ImGui::ColorPicker4("color", reinterpret_cast<float*>(&selection.grid_color.Value));
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("thickness"))
        {
          auto& selection = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
          ImGui::SliderFloat("##thickness", reinterpret_cast<float*>(&selection.grid_line_thickness), 1.f, 10.f);
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("delete", atlas_texture_is_set))
    {
      auto& selections = local_registry_.get<TileSetEditingState>(*editing_tile_set_id_);

      if (ImGui::MenuItem("selected", nullptr, false, selection_is_active))
      {
        // Remove from current tileset
        selections.erase(
          std::remove_if(
            selections.begin(),
            selections.end(),
            [id = *editing_tile_set_selection_id_](const auto& ref) { return ref.id == id; }),
          selections.end());
        // Remove selection
        local_registry_.destroy(*editing_tile_set_selection_id_);
        // Reset active selection
        editing_tile_set_selection_id_.reset();
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("all", nullptr, false, !selections.empty()))
      {
        // Remove all selection associate with tilset
        for (const auto& ref : selections)
        {
          local_registry_.destroy(*ref.id);
        }
        selections.clear();
        // Reset active selection
        editing_tile_set_selection_id_.reset();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("help"))
    {
      ImGui::TextUnformatted("  alt + {arrow} : resize selection grid");
      ImGui::TextUnformatted("shift + {arrow} : move one pixel");
      ImGui::TextUnformatted("        {arrow} : move one tile");
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }

  void AtlasTextureEditSelection(ImDrawList* drawlist, const ImTransform& screen_to_texture)
  {
    auto& GuiIO = ImGui::GetIO();

    auto [tile_name, tile_set, tile_set_selections, editing_state] =
      local_registry_.get<std::string, TileSet, TileSetEditingState, AtlasTextureEditingState>(*editing_tile_set_id_);

    // Handle drawing / mouse-over of selections
    {
      bool block_additional_hovering = false;
      for (const auto selection_ref : tile_set_selections)
      {
        auto& selection = resolve(local_registry_, selection_ref);
        const bool is_editing = selection_ref.id == editing_tile_set_selection_id_;
        const ImVec2 pos = screen_to_texture * selection.pos;
        const ImVec2 tsz = screen_to_texture ^ ToImVec2(tile_set.tile_size);
        const ImVec2 top = DrawGrid(
          drawlist,
          pos,
          tsz,
          selection.rows,
          selection.cols,
          is_editing ? ImFadeColor(selection.grid_color, 0.5f + 0.5f * time_elapsed_fadeosc_)
                     : ImFadeColor(selection.grid_color, 0.25f),
          screen_to_texture.scaling * selection.grid_line_thickness);

        if (!ImGui::IsMouseHoveringRect(pos, top) || block_additional_hovering)
        {
          continue;
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
          editing_tile_set_selection_id_ = selection_ref.id;
        }
        else
        {
          block_additional_hovering = true;
          static constexpr float kRectCornerRounding = 1.f;
          drawlist->AddRectFilled(
            pos,
            top,
            ImFadeColor(selection.grid_color, 0.5f * time_elapsed_fadeosc_),
            kRectCornerRounding,
            ImDrawFlags_None);
        }
      }
    }

    // Ignore if not focused
    // Ignore if no active selection or navigating
    if (!texture_atlas_is_hovered_ or !editing_tile_set_selection_id_ or GuiIO.KeyCtrl)
    {
      return;
    }

    // Snap selection to mouse position
    auto& selection = local_registry_.get<TileSetSelection>(*editing_tile_set_selection_id_);
    if (GuiIO.KeyAlt)
    {
      if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
      {
        selection.cols = std::max(1, selection.cols - 1);
      }
      if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
      {
        selection.cols = std::max(1, selection.cols + 1);
      }
      if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
      {
        selection.rows = std::max(1, selection.rows - 1);
      }
      if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
      {
        selection.rows = std::max(1, selection.rows + 1);
      }
    }
    else if (GuiIO.KeyShift)
    {
      if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
      {
        selection.pos.x -= 1;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
      {
        selection.pos.x += 1;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
      {
        selection.pos.y -= 1;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
      {
        selection.pos.y += 1;
      }
    }
    else
    {
      if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
      {
        selection.pos.x -= tile_set.tile_size.x();
      }
      if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
      {
        selection.pos.x += tile_set.tile_size.x();
      }
      if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
      {
        selection.pos.y -= tile_set.tile_size.y();
      }
      if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
      {
        selection.pos.y += tile_set.tile_size.y();
      }
    }
  }


  void TileSetCreateMenu()
  {
    if (ImGui::Button("new"))
    {
      tile_set_naming_pop_up_.open();
      ImGui::CloseCurrentPopup();
    }
    if (!editing_tile_set_id_.has_value())
    {
      return;
    }

    ImGui::SameLine();

    if (ImGui::Button("delete"))
    {
      tile_set_delete_confirmation_.open();
    }

    if (!tile_set_delete_confirmation_.is_open())
    {
      return;
    }

    if (tile_set_delete_confirmation_.is_confirmed())
    {
      local_registry_.destroy(*editing_tile_set_id_);
      editing_tile_set_id_.reset();
      editing_tile_set_selection_id_.reset();
    }
  }

  constexpr bool LockWindowMovement() const { return texture_atlas_is_hovered_; }

  using TileSetComponents = Components<
    std::string,
    TileSet,
    AtlasTextureEditingState,
    TileSetSelection,
    TileSetEditingState,
    Reference<Texture>>;

  template <typename OArchive> void Save(OArchive& ar) const
  {
    serializable_registry_t<const TileSetComponents> local_registry{local_registry_};
    ar << named{"local_registry", local_registry};
    ar << named{"editing_tile_set_id", editing_tile_set_id_};
    ar << named{"editing_tile_set_selection_id", editing_tile_set_selection_id_};
  }

  template <typename IArchive> void Load(IArchive& ar)
  {
    serializable_registry_t<TileSetComponents> local_registry{local_registry_};
    ar >> named{"local_registry", local_registry};
    ar >> named{"editing_tile_set_id", editing_tile_set_id_};
    ar >> named{"editing_tile_set_selection_id", editing_tile_set_selection_id_};
  }

private:
  float time_elapsed_seconds_ = 0.f;
  float time_elapsed_fadeosc_ = 0.f;
  bool texture_atlas_is_hovered_ = false;
  InputTextPopUp<100> tile_set_naming_pop_up_;
  InputTextPopUp<100> tile_set_rename_pop_up_;
  ConfirmationPopUp tile_set_delete_confirmation_;
  SubmissionPopUp tile_set_size_submission_pop_up_;
  std::optional<EntityID> editing_tile_set_id_;
  std::optional<EntityID> editing_tile_set_selection_id_;
  Registry local_registry_;
};

using namespace tyl::serialization;

TileSetCreator::~TileSetCreator() = default;

tyl::expected<TileSetCreator, ScriptCreationError> TileSetCreator::CreateImpl(const TileSetCreatorOptions& options)
{
  return TileSetCreator{options, std::make_unique<Impl>()};
}

TileSetCreator::TileSetCreator(const TileSetCreatorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

template <> void TileSetCreator::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void TileSetCreator::LoadImpl(ScriptIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

ScriptStatus TileSetCreator::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
  if (ImGui::Begin(
        options_.browser_name,
        nullptr,
        (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Browser(scene, shared, resources);
  }
  ImGui::End();

  if (ImGui::Begin(
        options_.creator_name,
        nullptr,
        (impl_->LockWindowMovement() ? ImGuiWindowFlags_NoMove : 0) | kStaticWindowFlags))
  {
    impl_->Creator(scene, shared, resources);
  }
  ImGui::End();
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::TileSetSelection> : std::true_type
{};

template <typename ArchiveT> struct serialize<ArchiveT, engine::AtlasTextureEditingState>
{
  void operator()(ArchiveT& ar, engine::AtlasTextureEditingState& editing_state)
  {
    ar& named{"show_grid", editing_state.show_grid};
    ar& named{"show_border", editing_state.show_border};
    ar& named{"show_source_filename", editing_state.show_source_filename};
    ar& named{"show_position", editing_state.show_position};
    ar& named{"zoom_sensivity", editing_state.zoom_sensivity};
    ar& named{"texture_tint", editing_state.texture_tint};
    ar& named{"window_to_texture", editing_state.window_to_texture};
    ar& named_ignored{"window_to_texture_on_nav_start", editing_state.window_to_texture_on_nav_start};
  }
};

}  // tyl::serialization
