/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drawing_2D.cpp
 */

// C++ Standard Library
#include <memory>
#include <variant>

// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/camera.hpp>
#include <tyl/engine/drawing.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/drawing_2D.hpp>
#include <tyl/format.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>

#include <tyl/rect.hpp>

namespace tyl::engine
{
namespace
{

enum class DrawNext : int
{
  kRectangle,
  kLineStrip,
  kITEMS
};

struct EditingRectangle
{
  EntityID id;
};

struct EditingLineStrip
{
  EntityID id;
};

struct Drawing2DProperties
{
  bool show_cursor_position = false;
  std::variant<std::monostate, EditingRectangle, EditingLineStrip> editing;
};

class EditingBehavior
{
public:
  explicit EditingBehavior(Registry& reg, Vec4f& active_color, const Vec2f& cursor_position) :
      reg_{&reg}, active_color_{&active_color}, cursor_position_{cursor_position}
  {}

  constexpr bool operator()([[maybe_unused]] std::monostate _) const { return false; };

  bool operator()(EditingRectangle& editing)
  {
    auto [rect, color] = reg_->get<Rect2D, Color>(editing.id);
    rect.max() = cursor_position_;
    color.rgba = *active_color_;
    return ImGui::IsMouseClicked(ImGuiMouseButton_Left);
  };

  bool operator()(EditingLineStrip& editing)
  {
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
      return true;
    }
    auto [linelist, color] = reg_->get<LineStrip2D, Color>(editing.id);
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || linelist.values.empty())
    {
      linelist.values.push_back(cursor_position_);
      linelist.values.push_back(cursor_position_);
    }
    else if (linelist.values.size() > 0)
    {
      linelist.values.back() = cursor_position_;
    }
    color.rgba = *active_color_;
    return false;
  };

private:
  Registry* reg_;
  const Vec4f* active_color_;
  Vec2f cursor_position_;
};

}  // namespace

using namespace tyl::serialization;

class Drawing2D::Impl
{
public:
  Impl() {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    allow_drawing_ = !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

    HandleShowCursorPosition(resources);
    {
      const char* items[] = {"Rectangle", "LineStrip"};
      int index = static_cast<int>(draw_next_);
      if (ImGui::BeginCombo("next shape", items[index]))
      {
        for (int n = 0; n < static_cast<int>(DrawNext::kITEMS); n++)
        {
          const bool is_selected = (index == n);
          if (ImGui::Selectable(items[n], is_selected))
          {
            index = n;
          }

          // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
          if (is_selected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      draw_next_ = static_cast<DrawNext>(index);
    }
    ImGui::ColorPicker3("color", active_color_.data());

    const auto& camera = scene.graphics.get<TopDownCamera2D>(*scene.active_camera);
    const auto inverse_camera_matrix = ToInverseCameraMatrix(camera);
    const Vec3f cursor_position_in_scene = inverse_camera_matrix *
      Vec3f{resources.viewport_cursor_position_normalized.x(), resources.viewport_cursor_position_normalized.y(), 1.f};

    if (std::holds_alternative<std::monostate>(properties_.editing))
    {
      if (!allow_drawing_)
      {
        // trap
      }
      else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      {
        const auto id = scene.graphics.create();
        switch (draw_next_)
        {
        case DrawNext::kRectangle: {
          scene.graphics.emplace<Rect2D>(id, cursor_position_in_scene.head<2>(), cursor_position_in_scene.head<2>());
          scene.graphics.emplace<Color>(id, Color{active_color_});
          properties_.editing = EditingRectangle{id};
          break;
        }
        case DrawNext::kLineStrip: {
          scene.graphics.emplace<LineStrip2D>(id);
          scene.graphics.emplace<Color>(id, Color{active_color_});
          properties_.editing = EditingLineStrip{id};
          break;
        }
        case DrawNext::kITEMS: {
          break;
        }
        }
      }
    }
    else if (
      std::visit(
        EditingBehavior{scene.graphics, active_color_, cursor_position_in_scene.head<2>()}, properties_.editing) &&
      allow_drawing_)
    {
      properties_.editing = std::monostate{};
    }
  }

  void HandleShowCursorPosition(const ScriptResources& resources)
  {
    ImGui::Checkbox("show cursor position", &properties_.show_cursor_position);
    if (properties_.show_cursor_position and !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
      auto* const viewport = ImGui::GetWindowViewport();
      auto* const drawlist = ImGui::GetWindowDrawList();
      drawlist->PushClipRect(viewport->WorkPos, viewport->WorkPos + viewport->WorkSize);
      drawlist->AddText(
        ToImVec2_(resources.viewport_cursor_position),
        IM_COL32_WHITE,
        format(
          "(%f, %f : %f, %f)",
          resources.viewport_cursor_position.x(),
          resources.viewport_cursor_position.y(),
          resources.viewport_cursor_position_normalized.x(),
          resources.viewport_cursor_position_normalized.y()));
      drawlist->PopClipRect();
    }
  }

  template <typename OArchive> void Save(OArchive& ar) const { ar << named{"properties", properties_}; }

  template <typename IArchive> void Load(IArchive& ar) { ar >> named{"properties", properties_}; }

private:
  bool allow_drawing_ = false;
  Vec4f active_color_ = {1, 1, 1, 1};
  DrawNext draw_next_ = DrawNext::kRectangle;
  Drawing2DProperties properties_ = {};
};

Drawing2D::~Drawing2D() = default;

template <> void Drawing2D::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const { impl_->Save(oar); }

template <> void Drawing2D::LoadImpl(ScriptIArchive<file_handle_istream>& iar) { impl_->Load(iar); }

tyl::expected<Drawing2D, ScriptCreationError> Drawing2D::CreateImpl(const Drawing2DOptions& options)
{
  return Drawing2D{options, std::make_unique<Impl>()};
}

Drawing2D::Drawing2D(const Drawing2DOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

ScriptStatus Drawing2D::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    if (scene.active_camera.has_value())
    {
      impl_->Update(scene, shared, resources);
    }
    else
    {
      ImGui::TextColored(ImVec4{1, 0, 0, 1}, "no active camera");
    }
  }
  ImGui::End();
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::Drawing2DProperties> : std::true_type
{};

}  // tyl::serialization