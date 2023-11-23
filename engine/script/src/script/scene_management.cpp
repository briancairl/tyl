/**
 * @copyright 2023-present Brian Cairl
 *
 * @file scene_management.cpp
 */

// C++ Standard Library
#include <string>

// Tyl
#include <tyl/engine/asset.hpp>
#include <tyl/engine/camera.hpp>
#include <tyl/engine/drawing.hpp>
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/scene_management.hpp>
#include <tyl/serialization/file_stream.hpp>

namespace tyl::serialization
{}  // tyl::serialization

namespace tyl::engine
{
using namespace tyl::serialization;

SceneManagement::~SceneManagement() = default;

tyl::expected<SceneManagement, ScriptCreationError> SceneManagement::CreateImpl(const SceneManagementOptions& options)
{
  return SceneManagement{options};
}

SceneManagement::SceneManagement(const SceneManagementOptions& options) : options_{options} {}

template <> void SceneManagement::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) {}

template <> void SceneManagement::LoadImpl(ScriptIArchive<file_handle_istream>& iar) {}

ScriptStatus SceneManagement::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    {
      static constexpr const char* kDefaultCameraName = "camera";
      static char kCameraLabelBuffer[100] = "camera";
      ImGui::InputText("name", kCameraLabelBuffer, sizeof(kCameraLabelBuffer));
      ImGui::SameLine();
      if (ImGui::Button("add"))
      {
        const auto camera = scene.graphics.create();
        scene.graphics.emplace<std::string>(camera, kCameraLabelBuffer);
        scene.graphics.emplace<TopDownCamera2D>(camera, Vec2f{0, 0}, 1.f, resources.viewport_size);
        scene.graphics.emplace<Rect2D>(camera, Vec2f{-1, -1}, Vec2f{1, 1});
        scene.graphics.emplace<Color>(camera, Vec4f{1, 1, 1, 1});
        scene.active_camera = camera;
        std::strcpy(kCameraLabelBuffer, kDefaultCameraName);
      }
    }
    ImGui::BeginChild("cameras");
    {
      ImGui::Separator();
      scene.graphics.view<TopDownCamera2D, Rect2D, std::string>().each(
        [&scene, &viewport_size = resources.viewport_size](
          EntityID id, TopDownCamera2D& camera, Rect2D& camera_rect, const std::string& label) {
          camera.viewport_size = viewport_size;

          ImGui::PushID(static_cast<int>(id));
          bool is_active = scene.active_camera == id;
          if (ImGui::Checkbox(label.c_str(), &is_active))
          {
            scene.active_camera = id;
          }

          ImGui::InputFloat2("translation", camera.translation.data());

          ImGui::InputFloat("scaling", &camera.scaling);

          if (ImGui::Button("delete"))
          {
            scene.graphics.destroy(id);
            if (is_active)
            {
              scene.active_camera.reset();
            }
          }
          ImGui::PopID();
          ImGui::Separator();
        });
      ImGui::EndChild();
    }
  }
  ImGui::End();
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine