/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
#include <cstring>
#include <memory>
#include <optional>

// Entt
#include <entt/entt.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/engine/widgets/tileset_creator.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/utility/dynamic_bitset.hpp>

namespace tyl::engine::widgets
{
namespace
{

constexpr bool kShowBorders = true;
constexpr float kScalingMin = 0.1f;
constexpr float kScalingMax = 10.0f;

}  // namespace

class TilesetCreator::Impl
{
public:
  Impl() {}

  void update(core::Resources& resources)
  {
    auto& registry = resources.registry;

    handle_texture_selection_creation_popup(registry);

    if (ImGui::BeginPopup(
          "#SelectionPicker",
          ImGuiWindowFlags_HorizontalScrollbar |
            (texture_id_.has_value() ? ImGuiWindowFlags_MenuBar : ImGuiWindowFlags_None)))
    {
      handle_texture_selection_creation_menu(registry);
      if (ImGui::BeginCombo("selection", next_selection_label_buffer_))
      {
        registry.view<SelectionTag, std::string>().each([this](const entt::entity id, const auto& label) {
          ImGui::PushID(static_cast<int>(id));
          if (ImGui::Selectable(label.c_str(), active_selection_ == id))
          {
            active_selection_ = id;
            std::strcpy(next_selection_label_buffer_, label.c_str());
          }
          ImGui::PopID();
        });
        ImGui::EndCombo();
      }

      if (active_selection_.has_value())
      {
        ImGui::SliderFloat("scaling", &scaling_, kScalingMin, kScalingMax);
        handle_texture_selection_grid_properties(registry, *active_selection_);
        handle_texture_selection_rect_properties(registry, *active_selection_);
      }
      ImGui::EndPopup();
    }

    ImGui::BeginChild("#DragAndDropArea", ImVec2{500, 500}, kShowBorders, ImGuiWindowFlags_HorizontalScrollbar);
    if (!texture_id_.has_value())
    {
      ImGui::Text("%s", "To start, drop a texture here!");
    }
    else if (!registry.valid(*texture_id_))
    {
      texture_id_.reset();
    }
    else
    {
      const auto origin = ImGui::GetCursorScreenPos();
      const auto& texture = registry.get<graphics::device::Texture>(*texture_id_);
      ImGui::Image(
        reinterpret_cast<void*>(texture.get_id()),
        ImVec2{scaling_ * texture.shape().height, scaling_ * texture.shape().width},
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImVec4(1, 1, 1, 1),
        ImVec4(0, 0, 0, 0));
      handle_texture_selection_interaction(origin, registry);
    }
    ImGui::EndChild();

    if (open_selection_picker_popup_ or ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup("#SelectionPicker");
      open_selection_picker_popup_ = false;
    }

    handle_texture_drag_and_drop(registry);

    ImGui::BeginChild("#TileSetPreviewArea", ImVec2{500, 250}, kShowBorders, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::EndChild();
  }

  constexpr bool lock_window_movement() const { return is_dragging_offset_; }

private:
  struct SelectionTag
  {};

  struct SelectionRect
  {
    ImVec2 offset = {0.f, 0.f};
    ImVec2 size = {1.f, 1.f};
  };

  struct SelectionGrid
  {
    ImVec2 offset = {0.f, 0.f};
    ImVec2 cell_size = {25.f, 25.f};
    int dims[2] = {10, 10};
    dynamic_bitset<std::uint64_t> cell_selected;

    SelectionGrid(const int rows, const int cols, const ImVec2 _cell_size) : cell_size{_cell_size}
    {
      dims[0] = rows;
      dims[1] = cols;
      cell_selected.resize(rows * cols);
    }
  };

  void handle_texture_drag_and_drop(entt::registry& registry)
  {
    if (!ImGui::BeginDragDropTarget())
    {
      return;
    }
    else if (const auto* const payload = ImGui::AcceptDragDropPayload("_TEXTURE_ASSET", /*cond = */ 0);
             payload != nullptr)
    {
      const core::asset::Path asset_path{
        std::string_view{reinterpret_cast<char*>(payload->Data), static_cast<std::size_t>(payload->DataSize)}};
      if (auto guid_or_error = core::asset::get(registry, asset_path); guid_or_error.has_value())
      {
        texture_id_.emplace(std::move(guid_or_error).value());
        active_selection_.reset();
      }
      else
      {
        // TODO(qol) handle error
      }
    }
    ImGui::EndDragDropTarget();
  }

  void handle_texture_selection_creation_menu(entt::registry& registry)
  {
    if (!texture_id_.has_value())
    {
      return;
    }

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("create"))
      {
        if (ImGui::MenuItem("new rect"))
        {
          next_selection_shape_ = NextSelectionShape::kRect;
          open_selection_label_popup_ = true;
        }
        if (ImGui::MenuItem("new grid"))
        {
          next_selection_shape_ = NextSelectionShape::kGrid;
          open_selection_label_popup_ = true;
        }
        ImGui::EndMenu();
      }

      if (active_selection_.has_value() and ImGui::BeginMenu("edit"))
      {
        if (ImGui::MenuItem("delete"))
        {
          registry.destroy(*active_selection_);
          active_selection_.reset();

          auto view = registry.view<SelectionTag, std::string>();
          for (const auto id : view)
          {
            active_selection_ = id;
            std::strcpy(next_selection_label_buffer_, registry.get<std::string>(id).c_str());
          }

          if (!active_selection_.has_value())
          {
            std::strcpy(next_selection_label_buffer_, "no selections");
          }
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
  }

  void handle_texture_selection_creation_popup(entt::registry& registry)
  {
    static constexpr const char* kCreationPopupModalName = "selection_menu";

    if (open_selection_label_popup_)
    {
      ImGui::OpenPopup(kCreationPopupModalName);
      open_selection_label_popup_ = false;
    }

    if (ImGui::BeginPopupModal(
          kCreationPopupModalName, nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar))
    {
      if (ImGui::InputText(
            "label",
            next_selection_label_buffer_,
            sizeof(next_selection_label_buffer_),
            ImGuiInputTextFlags_EnterReturnsTrue))
      {
        const auto& texture = registry.get<graphics::device::Texture>(*texture_id_);
        const auto id = registry.create();
        switch (*next_selection_shape_)
        {
        case NextSelectionShape::kRect: {
          registry.emplace<std::string>(id, next_selection_label_buffer_);
          registry.emplace<SelectionTag>(id);
          {
            auto& select = registry.emplace<SelectionRect>(id);
            select.size.x = texture.shape().height;
            select.size.y = texture.shape().width;
          }
          break;
        }
        case NextSelectionShape::kGrid: {
          registry.emplace<std::string>(id, next_selection_label_buffer_);
          registry.emplace<SelectionTag>(id);
          registry.emplace<SelectionGrid>(id, 10, 10, ImVec2(texture.shape().height / 10, texture.shape().width / 10));
          registry.emplace<ImColor>(id, 1.f, 1.f, 0.f, 1.f);
          break;
        }
        }
        active_selection_ = id;
        ImGui::CloseCurrentPopup();
        open_selection_picker_popup_ = true;
      }
      ImGui::EndPopup();
    }
  }

  static void handle_texture_selection_rect_properties(entt::registry& registry, const entt::entity id)
  {
    if (!registry.all_of<SelectionRect>(id))
    {
      return;
    }

    auto& properties = registry.get<SelectionRect>(id);

    if (ImGui::InputFloat2("offset", reinterpret_cast<float*>(&properties.offset)))
    {
      properties.offset.x = std::max(0.f, properties.offset.x);
      properties.offset.y = std::max(0.f, properties.offset.y);
    }

    if (ImGui::InputFloat2("size", reinterpret_cast<float*>(&properties.size)))
    {
      properties.size.x = std::max(1.f, properties.size.x);
      properties.size.y = std::max(1.f, properties.size.y);
    }
  }

  void
  handle_texture_selection_rect_interaction(ImDrawList* const drawlist, const ImVec2& origin, entt::registry& registry)
  {
    registry.view<std::string, SelectionRect>().each(
      [&](const entt::entity selection_id, const auto& label, auto& rect) {
        const float ox = origin.x + rect.offset.x * scaling_;
        const float oy = origin.y + rect.offset.y * scaling_;
        const float cx = rect.size.x * scaling_;
        const float cy = rect.size.y * scaling_;
        const bool is_selected = active_selection_ == selection_id;

        // Corner dragging
        if (is_selected)
        {
          static constexpr float kGrabRadius = 10.f;
          ImVec2 lower{ox - kGrabRadius, oy - kGrabRadius};
          ImVec2 upper{ox + kGrabRadius, oy + kGrabRadius};
          const bool is_hovering = ImGui::IsMouseHoveringRect(lower, upper);
          const bool is_mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

          is_dragging_offset_ = (is_dragging_offset_ and is_mouse_down) or is_hovering;

          if (is_dragging_offset_ and is_mouse_down)
          {
            const auto mouse_pos_relative = ImGui::GetMousePos() - origin;
            const auto inv_scaling = 1.f / scaling_;
            rect.offset.x = std::max(0.f, std::round(mouse_pos_relative.x * inv_scaling));
            rect.offset.y = std::max(0.f, std::round(mouse_pos_relative.y * inv_scaling));
          }
          else if (is_hovering)
          {
            drawlist->AddRectFilled(lower, upper, IM_COL32(255, 0, 255, 100));
          }
        }

        {
          const float line_thickness = is_selected ? std::max(1.f, 2.f * scaling_) : 1.f;
          const auto line_color = is_selected ? IM_COL32(255, 255, 25, 255) : IM_COL32(255, 100, 50, 50);

          const ImVec2 lower{ox, oy};
          const ImVec2 upper{lower.x + cx, lower.y + cy};
          drawlist->AddRect(lower, upper, line_color, 0.0f, 0, line_thickness);
        }
      });
  }

  static void handle_texture_selection_grid_properties(entt::registry& registry, const entt::entity id)
  {
    if (!registry.all_of<SelectionGrid>(id))
    {
      return;
    }

    auto [properties, color] = registry.get<SelectionGrid, ImColor>(id);

    if (ImGui::InputInt2("dims", properties.dims, ImGuiInputTextFlags_EnterReturnsTrue))
    {
      properties.dims[0] = std::max(1, properties.dims[0]);
      properties.dims[1] = std::max(1, properties.dims[1]);
      properties.cell_selected.resize(properties.dims[0] * properties.dims[1], false);
    }

    if (ImGui::InputFloat2("offset", reinterpret_cast<float*>(&properties.offset)))
    {
      properties.offset.x = std::max(0.f, properties.offset.x);
      properties.offset.y = std::max(0.f, properties.offset.y);
    }

    if (ImGui::InputFloat2("cell size", reinterpret_cast<float*>(&properties.cell_size)))
    {
      properties.cell_size.x = std::max(1.f, properties.cell_size.x);
      properties.cell_size.y = std::max(1.f, properties.cell_size.y);
    }

    if (ImGui::Button("clear selection"))
    {
      properties.cell_selected.fill(false);
    }

    ImGui::ColorPicker4("grid color", reinterpret_cast<float*>(&color.Value));
  }

  void
  handle_texture_selection_grid_interaction(ImDrawList* const drawlist, const ImVec2& origin, entt::registry& registry)
  {
    registry.view<std::string, ImColor, SelectionGrid>().each(
      [&](const entt::entity selection_id, const auto& label, const auto& grid_color, auto& grid) {
        const float ox = origin.x + grid.offset.x * scaling_;
        const float oy = origin.y + grid.offset.y * scaling_;
        const float cx = grid.cell_size.x * scaling_;
        const float cy = grid.cell_size.y * scaling_;
        const bool is_selected = active_selection_ == selection_id;

        // Corner dragging
        if (is_selected)
        {
          static constexpr float kGrabRadius = 10.f;
          ImVec2 lower{ox - kGrabRadius, oy - kGrabRadius};
          ImVec2 upper{ox + kGrabRadius, oy + kGrabRadius};
          const bool is_hovering = ImGui::IsMouseHoveringRect(lower, upper);
          const bool is_mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

          is_dragging_offset_ = (is_dragging_offset_ and is_mouse_down) or is_hovering;

          if (is_dragging_offset_ and is_mouse_down)
          {
            const auto mouse_pos_relative = ImGui::GetMousePos() - origin;
            const auto inv_scaling = 1.f / scaling_;
            grid.offset.x = std::max(0.f, std::round(mouse_pos_relative.x * inv_scaling));
            grid.offset.y = std::max(0.f, std::round(mouse_pos_relative.y * inv_scaling));
          }
          else if (is_hovering)
          {
            drawlist->AddRectFilled(lower, upper, IM_COL32(255, 0, 255, 100));
          }
        }

        // Handle cell selection
        {
          int s = 0;
          for (int i = 0; i < grid.dims[1]; ++i)
          {
            for (int j = 0; j < grid.dims[0]; ++j, ++s)
            {
              const ImVec2 lower{ox + i * cx, oy + j * cy};
              const ImVec2 upper{lower.x + cx, lower.y + cy};

              if (grid.cell_selected[s])
              {
                drawlist->AddRectFilled(lower, upper, IM_COL32(255, 50, 25, 100));
              }

              if (is_dragging_offset_ or !is_selected or !ImGui::IsMouseHoveringRect(lower, upper))
              {
                continue;
              }
              else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
              {
                grid.cell_selected.flip(s);
              }
              else
              {
                drawlist->AddRectFilled(lower, upper, ImGui::GetColorU32(grid_color.Value));
              }
            }
          }
        }

        // Draw grid lines
        {
          const float gy = grid.dims[0] * cy;
          const float gx = grid.dims[1] * cx;
          const float line_thickness = is_selected ? std::max(1.f, 2.f * scaling_) : 1.f;
          const auto line_color =
            is_selected ? ImGui::GetColorU32(grid_color.Value) : (ImGui::GetColorU32(grid_color.Value) / 4);
          for (int i = 0; i <= grid.dims[0]; ++i)
          {
            drawlist->AddLine(ImVec2{ox, oy + i * cy}, ImVec2{ox + gx, oy + i * cy}, line_color, line_thickness);
          }
          for (int i = 0; i <= grid.dims[1]; ++i)
          {
            drawlist->AddLine(ImVec2{ox + i * cx, oy}, ImVec2{ox + i * cx, oy + gy}, line_color, line_thickness);
          }
        }
      });
  }

  void handle_texture_selection_interaction(const ImVec2& origin, entt::registry& registry)
  {
    auto* const drawlist = ImGui::GetWindowDrawList();
    handle_texture_selection_grid_interaction(drawlist, origin, registry);
    handle_texture_selection_rect_interaction(drawlist, origin, registry);
  }

  enum class NextSelectionShape
  {
    kRect,
    kGrid
  };

  float scaling_ = 1.f;
  std::optional<entt::entity> texture_id_;
  std::optional<entt::entity> active_selection_;
  std::optional<NextSelectionShape> next_selection_shape_;
  bool open_selection_picker_popup_ = false;
  bool open_selection_label_popup_ = false;
  char next_selection_label_buffer_[100] = "no selections";
  bool is_dragging_offset_ = false;
  // bool is_dragging_size_ = false;
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
  if (ImGui::Begin(
        options_.name,
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | (impl_->lock_window_movement() ? ImGuiWindowFlags_NoMove : 0)))
  {
    impl_->update(resources);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
