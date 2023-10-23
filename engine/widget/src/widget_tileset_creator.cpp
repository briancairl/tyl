/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.cpp
 */

// C++ Standard Library
#include <memory>
#include <optional>
#include <vector>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/internal/drag_and_drop_images.hpp>
#include <tyl/engine/internal/imgui.hpp>
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

struct TileSetSelection
{
  int rows = 10;
  int cols = 10;
  ImVec2 pos = {0.f, 0.f};

  ImColor grid_color = IM_COL32_WHITE;
  float grid_line_thickness = 1.f;
};

using TileSetSelections = std::vector<Reference<TileSetSelection>>;

struct AtlasTextureEditingState
{
  bool show_grid = true;
  bool show_border = false;
  bool show_source_filename = false;
  bool show_position = true;
  float zoom_sensivity = 1e-1f;
  ImTransform window_to_texture;
  std::optional<ImTransform> window_to_texture_on_nav_start = std::nullopt;
};

struct TileSet
{
  Vec2f tile_size = {16, 16};
  std::vector<Rect2f> tiles;
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
    time_elapsed_seconds_ += ImGui::GetIO().DeltaTime;
    time_elapsed_fadeosc_ = std::abs(std::sin(2.f * time_elapsed_seconds_));
    AtlasTexturePreview(registry, shared, resources);
    TileSetSubmitSelections(registry, resources);
  }

  void TileSetSubmitSelections(Registry& registry, const WidgetResources& resources)
  {
    if (
      !texture_atlas_is_hovered_ or !editing_tile_set_id_ or
      !registry.any_of<Reference<Texture>>(*editing_tile_set_id_) or !ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      return;
    }

    const auto [texture_ref, tile_set, tile_set_selections] =
      registry.get<Reference<Texture>, TileSet, TileSetSelections>(*editing_tile_set_id_);
    if (tile_set_selections.empty())
    {
      return;
    }

    tile_set.tiles.clear();

    const auto& texture = resolve(registry, texture_ref);
    for (const auto& selection_ref : tile_set_selections)
    {
      const auto& selection = resolve(registry, selection_ref);
      const auto tile_size = ToImVec2(tile_set.tile_size);
      for (int i = 0; i < selection.cols; ++i)
      {
        for (int j = 0; j < selection.rows; ++j)
        {
          const ImVec2 min_pt = selection.pos + ImVec2{i * tile_size.x, j * tile_size.y};
          const ImVec2 max_pt = min_pt + tile_size;
          const ImVec2 min_pt_uv{min_pt.x / texture.shape().height, min_pt.y / texture.shape().width};
          const ImVec2 max_pt_uv{max_pt.x / texture.shape().height, max_pt.y / texture.shape().width};
          tile_set.tiles.emplace_back(FromImVec2(min_pt_uv), FromImVec2(max_pt_uv));
        }
      }
    }
  }

  void TileSetPreview(Registry& registry, const WidgetResources& resources)
  {
    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_None;
    ImGui::BeginChild("##TileSetPreview", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);
    if (ImGui::BeginTable("##TileSetPreviewTable", 2, ImGuiTableFlags_Resizable))
    {
      registry.view<std::string, TileSet>().each([&](EntityID id, const auto& label, auto& tile_set) {
        const auto* atlas_texture_ref = registry.try_get<Reference<Texture>>(id);

        if (ImGui::TableNextColumn())
        {
          ImGui::PushID(static_cast<int>(id));

          ImGui::SeparatorText(label.c_str());
          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
            editing_tile_set_id_ = id;
          }
          if (ImGui::InputFloat2("tile size", tile_set.tile_size.data()))
          {
            TileSetSubmitSelections(registry, resources);
          }
          ImGui::PopID();
        }

        if (editing_tile_set_id_ == id)
        {
          ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor{1.f, 1.f, 0.f, 0.25f});
        }

        if (ImGui::TableNextColumn())
        {
          ImGui::SeparatorText(ImFmt("tiles: %lu", tile_set.tiles.size()));
          if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
          {
            editing_tile_set_id_ = id;
          }
          if (atlas_texture_ref == nullptr)
          {
            return;
          }
          else if (auto* atlas_texture = maybe_resolve(registry, *atlas_texture_ref); atlas_texture != nullptr)
          {
            ImTileSmallPreview(tile_set, *atlas_texture, ImVec2{0, 50});
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
              editing_tile_set_id_ = id;
            }
          }
        }
      });
      ImGui::EndTable();
    }
    ImGui::EndChild();
  }

  void AtlasTextureDragAndDropInternalSink(Registry& registry)
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
        *editing_tile_set_id_, *reinterpret_cast<const EntityID*>(texture_payload->Data));
    }
    ImGui::EndDragDropTarget();
  }

  void
  AtlasTextureDragAndDropExternalSink(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    if (!editing_tile_set_id_ or registry.any_of<Reference<Texture>>(*editing_tile_set_id_))
    {
      return;
    }

    // Handle drag/drop
    const auto loaded_texture_ids_or_error = drag_and_drop_images_.update(
      registry, shared, resources, [is_hovered = texture_atlas_is_hovered_] { return is_hovered; });

    // Create reference to first loaded texture
    if (loaded_texture_ids_or_error.has_value() and !loaded_texture_ids_or_error->empty())
    {
      registry.emplace<Reference<Texture>>(*editing_tile_set_id_, loaded_texture_ids_or_error->front());
    }
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

  void AtlasTexturePreview(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    if (!editing_tile_set_id_)
    {
      return;
    }

    static constexpr bool kChildShowBoarders = false;
    static constexpr auto kChildFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild("#AtlasTexture", ImVec2{0, 0}, kChildShowBoarders, kChildFlags);

    const ImTransform screen_to_window{ImGui::GetWindowPos()};
    const ImTransform screen_to_pointer{ImGui::GetMousePos()};

    auto* drawlist = ImGui::GetWindowDrawList();
    auto& editing_state = registry.get<AtlasTextureEditingState>(*editing_tile_set_id_);

    if (ImGui::IsWindowHovered())
    {
      const auto view_to_pointer = ImInverse(screen_to_window) * screen_to_pointer;
      AtlasTextureNav(view_to_pointer, editing_state);
    }

    const ImTransform screen_to_texture = screen_to_window * editing_state.window_to_texture;

    if (registry.any_of<Reference<Texture>>(*editing_tile_set_id_))
    {
      auto texture_ref = registry.get<Reference<Texture>>(*editing_tile_set_id_);
      if (const auto* texture = maybe_resolve(registry, texture_ref); texture == nullptr)
      {
        registry.remove<Reference<Texture>>(*editing_tile_set_id_);
      }
      else
      {
        const ImVec2 texture_size{
          static_cast<float>(texture->shape().height), static_cast<float>(texture->shape().width)};
        const ImVec2 texture_min_corner = screen_to_texture * ImVec2{0, 0};
        const ImVec2 texture_max_corner = screen_to_texture * texture_size;

        drawlist->AddImage(reinterpret_cast<void*>(texture->get_id()), texture_min_corner, texture_max_corner);

        if (editing_state.show_grid)
        {
          auto& tile_set = registry.get<TileSet>(*editing_tile_set_id_);
          const int rows = texture->shape().width / tile_set.tile_size.y();
          const int cols = texture->shape().height / tile_set.tile_size.x();
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
          const auto& filename = registry.get<std::filesystem::path>(texture_ref.id);
          drawlist->AddText(
            texture_min_corner + ImVec2{0, -20},
            ImColor{ImGui::GetStyle().Colors[ImGuiCol_Text]},
            filename.string().c_str());
        }
      }
    }
    else
    {
      const auto& label = registry.get<std::string>(*editing_tile_set_id_);
      const auto* text = ImFmt("DROP TEXTURE HERE FOR [%s]", label.c_str());
      drawlist->AddText(
        screen_to_window.offset + (ImGui::GetContentRegionAvail() - ImGui::CalcTextSize(text)) * 0.5f,
        ImColor{ImFadeColor(ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget], time_elapsed_fadeosc_)},
        text);
    }

    ImGui::EndChild();
    texture_atlas_is_hovered_ = ImGui::IsItemHovered();

    const auto texture_to_screen = ImInverse(screen_to_texture);
    const auto texture_to_pointer = texture_to_screen * screen_to_pointer;

    if (editing_state.show_position)
    {
      const auto* text = ImFmt(
        "(%f, %f) [%.3f%%]",
        texture_to_pointer.offset.x,
        texture_to_pointer.offset.y,
        100.f / texture_to_pointer.scaling);
      drawlist->AddText(screen_to_pointer.offset, ImColor{ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]}, text);
    }

    AtlasTexturePopUp(registry, resources, texture_to_screen);
    AtlasTextureDragAndDropExternalSink(registry, shared, resources);
    AtlasTextureDragAndDropInternalSink(registry);
    AtlasTextureEditSelection(registry, resources, drawlist, screen_to_texture);
  }

  void AtlasTexturePopUp(Registry& registry, const WidgetResources& resources, const ImTransform& texture_to_screen)
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
    const bool texture_is_specified = registry.any_of<Reference<Texture>>(*editing_tile_set_id_);
    const bool selection_is_active = static_cast<bool>(editing_tile_set_selection_id_);

    if (ImGui::BeginMenu("settings", texture_is_specified))
    {
      auto& editing_state = registry.get<AtlasTextureEditingState>(*editing_tile_set_id_);
      ImGui::Checkbox("show grid", &editing_state.show_grid);
      ImGui::Checkbox("show border", &editing_state.show_border);
      ImGui::Checkbox("show source", &editing_state.show_source_filename);
      ImGui::Checkbox("show position", &editing_state.show_position);
      ImGui::SliderFloat("zoom sensitivity", &editing_state.zoom_sensivity, 1e-3f, 5e-1f);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("create", texture_is_specified))
    {
      if (ImGui::MenuItem("new", nullptr, false, true))
      {
        // Create new selection
        const auto id = registry.create();
        registry.emplace<TileSetSelection>(id).pos = ImTruncate(texture_to_screen * screen_popup_pos);
        // Add to current tileset
        registry.get<TileSetSelections>(*editing_tile_set_id_).emplace_back(Reference<TileSetSelection>{id});
        // Set active selection
        editing_tile_set_selection_id_ = id;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("duplicate", nullptr, false, selection_is_active))
      {
        // Get selection we want to duplicate
        const auto& selection_to_copy = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
        // Create new selection
        const auto id = registry.create();
        registry.emplace<TileSetSelection>(id, selection_to_copy).pos = texture_to_screen * screen_popup_pos;
        // Add to current tileset
        registry.get<TileSetSelections>(*editing_tile_set_id_).emplace_back(Reference<TileSetSelection>{id});
        // Set active selection
        editing_tile_set_selection_id_ = id;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("selection", texture_is_specified))
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
        auto& selection = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
        selection.pos = texture_to_screen * screen_popup_pos;
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::BeginMenu("properties", selection_is_active))
      {
        auto& selection = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
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
          auto& selection = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
          ImGui::ColorPicker4("color", reinterpret_cast<float*>(&selection.grid_color.Value));
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("thickness"))
        {
          auto& selection = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
          ImGui::SliderFloat("##thickness", reinterpret_cast<float*>(&selection.grid_line_thickness), 1.f, 10.f);
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("delete", texture_is_specified))
    {
      auto& selections = registry.get<TileSetSelections>(*editing_tile_set_id_);

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
        registry.destroy(*editing_tile_set_selection_id_);
        // Reset active selection
        editing_tile_set_selection_id_.reset();
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("all", nullptr, false, !selections.empty()))
      {
        // Remove all selection associate with tilset
        for (const auto& ref : selections)
        {
          registry.destroy(ref.id);
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

  void AtlasTextureEditSelection(
    Registry& registry,
    const WidgetResources& resources,
    ImDrawList* drawlist,
    const ImTransform& screen_to_texture)
  {
    auto& GuiIO = ImGui::GetIO();

    auto [tile_name, tile_set, tile_set_selections, editing_state] =
      registry.get<std::string, TileSet, TileSetSelections, AtlasTextureEditingState>(*editing_tile_set_id_);

    // Handle drawing / mouse-over of selections
    {
      bool block_additional_hovering = false;
      for (const auto selection_ref : tile_set_selections)
      {
        auto& selection = registry.get<TileSetSelection>(selection_ref.id);
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
    auto& selection = registry.get<TileSetSelection>(*editing_tile_set_selection_id_);
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

      if (ImGui::MenuItem("delete") and editing_tile_set_id_)
      {
        registry.destroy(*editing_tile_set_id_);
        editing_tile_set_id_.reset();
        editing_tile_set_selection_id_.reset();
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
        registry.emplace<AtlasTextureEditingState>(id);
        registry.emplace<TileSetSelections>(id);
        editing_tile_set_id_ = id;
      }
      std::strcpy(kTileSetNameBuffer, kTileSetNameBufferDefault);
      tile_set_naming_pop_up_open_ = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  constexpr bool LockWindowMovement() const { return texture_atlas_is_hovered_; }

private:
  float time_elapsed_seconds_ = 0.f;
  float time_elapsed_fadeosc_ = 0.f;
  bool texture_atlas_is_hovered_ = false;
  bool tile_set_naming_pop_up_open_ = false;
  std::optional<EntityID> editing_tile_set_id_;
  std::optional<EntityID> editing_tile_set_selection_id_;
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