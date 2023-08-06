/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tileset_creator.cpp
 */

// C++ Standard Library
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

  void update(entt::registry& registry)
  {
    if (ImGui::BeginPopup("#SelectionPicker", ImGuiWindowFlags_HorizontalScrollbar))
    {
      handle_texture_selection_creation(registry);
      ImGui::SliderFloat("scaling", &scaling_, kScalingMin, kScalingMax);

      ImGui::BeginChild("#SelectionTabs", ImVec2{0, 150}, kShowBorders, ImGuiWindowFlags_HorizontalScrollbar);
      if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_Reorderable))
      {
        registry.view<SelectionTag, std::string>().each([this](const entt::entity id, const auto& label) {
          ImGui::PushID(static_cast<int>(id));
          if (ImGui::BeginTabItem(label.c_str(), nullptr, ImGuiTabItemFlags_None))
          {
            active_selection_ = id;
            ImGui::EndTabItem();
          }
          ImGui::PopID();
        });
        ImGui::EndTabBar();
      }
      handle_texture_selection_grid_properties(registry);
      handle_texture_selection_rect_properties(registry);
      ImGui::EndChild();

      if (active_selection_.has_value() && ImGui::Button("delete current"))
      {
        registry.destroy(*active_selection_);
        active_selection_.reset();
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

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      ImGui::OpenPopup("#SelectionPicker");
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

  void handle_texture_selection_creation(entt::registry& registry)
  {
    if (!texture_id_.has_value())
    {
      return;
    }

    if (ImGui::Button("new rect"))
    {
      const auto& texture = registry.get<graphics::device::Texture>(*texture_id_);
      const auto id = registry.create();
      registry.emplace<std::string>(id, "rect");
      registry.emplace<SelectionTag>(id);
      {
        auto& select = registry.emplace<SelectionRect>(id);
        select.size.x = texture.shape().height;
        select.size.y = texture.shape().width;
      }
      active_selection_ = id;
    }

    if (ImGui::Button("new grid"))
    {
      const auto& texture = registry.get<graphics::device::Texture>(*texture_id_);
      const auto id = registry.create();
      registry.emplace<std::string>(id, "grid");
      registry.emplace<SelectionTag>(id);
      registry.emplace<SelectionGrid>(id, 10, 10, ImVec2(texture.shape().height / 10, texture.shape().width / 10));
      active_selection_ = id;
    }
  }

  void handle_texture_selection_rect_properties(entt::registry& registry)
  {
    if (!active_selection_.has_value() or !registry.all_of<SelectionRect>(*active_selection_))
    {
      return;
    }

    auto& properties = registry.get<SelectionRect>(*active_selection_);

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

  void handle_texture_selection_grid_properties(entt::registry& registry)
  {
    if (!active_selection_.has_value() or !registry.all_of<SelectionGrid>(*active_selection_))
    {
      return;
    }

    auto& properties = registry.get<SelectionGrid>(*active_selection_);

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
  }

  void
  handle_texture_selection_grid_interaction(ImDrawList* const drawlist, const ImVec2& origin, entt::registry& registry)
  {
    registry.view<std::string, SelectionGrid>().each(
      [&](const entt::entity selection_id, const auto& label, auto& grid) {
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
                drawlist->AddRectFilled(lower, upper, IM_COL32(255, 255, 0, 100));
              }
            }
          }
        }

        // Draw grid lines
        {
          const float gy = grid.dims[0] * cy;
          const float gx = grid.dims[1] * cx;
          const float line_thickness = is_selected ? std::max(1.f, 2.f * scaling_) : 1.f;
          const auto line_color = is_selected ? IM_COL32(255, 255, 25, 255) : IM_COL32(255, 100, 50, 50);
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

  float scaling_ = 1.f;
  std::optional<entt::entity> texture_id_;
  std::optional<entt::entity> active_selection_;
  bool is_dragging_offset_ = false;
  bool is_dragging_size_ = false;
};

TilesetCreator::~TilesetCreator() = default;

tyl::expected<TilesetCreator, TilesetCreator::OnCreateErrorCode> TilesetCreator::create(const Options& options)
{
  return TilesetCreator{options, std::make_unique<Impl>()};
}

TilesetCreator::TilesetCreator(const Options& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

void TilesetCreator::update(ImGuiContext* const imgui_ctx, entt::registry& reg)
{
  ImGui::ShowDemoWindow();
  ImGui::SetCurrentContext(imgui_ctx);
  if (ImGui::Begin(
        options_.name,
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | (impl_->lock_window_movement() ? ImGuiWindowFlags_NoMove : 0)))
  {
    impl_->update(reg);
  }
  ImGui::End();
}

}  // namespace tyl::engine::widgets
