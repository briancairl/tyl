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

struct EditingRectangle
{
  EntityID id;
};

struct Drawing2DProperties
{
  bool show_cursor_position = false;
  std::variant<std::monostate, EditingRectangle> editing;
};

class EditingBehavior
{
public:
  explicit EditingBehavior(Registry& reg, const Vec2f& cursor_position) : reg_{&reg}, cursor_position_{cursor_position}
  {}

  constexpr bool operator()([[maybe_unused]] std::monostate _) const { return false; };

  bool operator()(EditingRectangle& editing)
  {
    auto [rect, color] = reg_->get<Rect2D, Color>(editing.id);
    ImGui::ColorPicker3("rectangle color", color.rgba.data());
    rect.max() = cursor_position_;
    return ImGui::IsMouseClicked(ImGuiMouseButton_Left);
  };

private:
  Registry* reg_;
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

    ShowCursorPosition(resources);

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
        scene.graphics.emplace<Rect2D>(id, cursor_position_in_scene.head<2>(), cursor_position_in_scene.head<2>());
        scene.graphics.emplace<Color>(id, Color{{1, 1, 1, 1}});
        properties_.editing = EditingRectangle{id};
      }
    }
    else if (
      std::visit(EditingBehavior{scene.graphics, cursor_position_in_scene.head<2>()}, properties_.editing) &&
      allow_drawing_)
    {
      properties_.editing = std::monostate{};
    }
  }

  void ShowCursorPosition(const ScriptResources& resources)
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