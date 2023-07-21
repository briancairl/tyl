/**
 * @copyright 2022-present Brian Cairl
 *
 * @file editor.cpp
 */


// C++ Standard Library
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>
#include <type_traits>
#include <unordered_map>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/app.hpp>
#include <tyl/engine/core/resource.hpp>
#include <tyl/engine/graphics/primitives_renderer.hpp>
#include <tyl/engine/graphics/types.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/render_target.hpp>
#include <tyl/graphics/device/render_target_texture.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/utility/expected.hpp>

using namespace tyl::engine;
using namespace tyl::graphics;

struct TextureDisplayProperties
{
  static constexpr float kMinZoom = 0.1f;
  static constexpr float kMaxZoom = 10.f;
  float zoom = kMinZoom;
};

struct DefaultTextureLocator : core::resource::Texture::Locator
{
  bool load(entt::registry& reg, const entt::entity id, const core::resource::Path& path) const override
  {
    auto image_or_error = host::Image::load(path.string().c_str());
    if (image_or_error.has_value())
    {
      reg.emplace<device::Texture>(id, image_or_error->texture());
      reg.emplace<TextureDisplayProperties>(id, TextureDisplayProperties{});
      return true;
    }
    else
    {
      return false;
    }
  }
};


int main(int argc, char** argv)
{
  auto app = tyl::engine::core::App::create({
    .initial_window_height = 500,
    .initial_window_width = 500,
    .window_title = "engine",
    .enable_vsync = true,
  });
  if (!app.has_value())
  {
    std::cerr << app.error() << std::endl;
    return 1;
  }

  entt::locator<core::resource::Texture::Locator>::emplace<DefaultTextureLocator>();

  entt::registry registry;

  {
    const auto id = registry.create();

    registry.emplace<graphics::DrawType::LineStrip>(id);
    registry.emplace<graphics::VertexColor>(id, 1.0f, 0.0f, 0.0f, 1.0f);

    {
      auto& vertices = registry.emplace<graphics::VertexList2D>(id);
      vertices.emplace_back(+0.5f, +0.0f);
      vertices.emplace_back(+0.5f, +0.5f);
      vertices.emplace_back(-0.5f, -0.0f);
      vertices.emplace_back(-0.5f, -0.5f);
    }
  }


  {
    const auto id = registry.create();

    registry.emplace<graphics::DrawType::LineStrip>(id);
    registry.emplace<graphics::VertexColor>(id, 1.0f, 0.0f, 1.0f, 1.0f);

    {
      auto& vertices = registry.emplace<graphics::VertexList2D>(id);
      vertices.emplace_back(+0.8f, +0.0f);
      vertices.emplace_back(+0.8f, +0.8f);
      vertices.emplace_back(-0.8f, -0.0f);
      vertices.emplace_back(-0.8f, -0.8f);
    }
  }


  auto rtt = device::RenderTargetTexture::create({200, 200});

  auto primitives_renderer = graphics::PrimitivesRenderer::create({.max_vertex_count = 100});
  if (!primitives_renderer.has_value())
  {
    return 1;
  }

  graphics::TopDownCamera2D camera{
    .translation = {-0.0f, 0.0f},
    .scaling = 5.0f,
  };


  const auto update_callback = [&](const tyl::engine::core::App::State& app_state) {
    if (app_state.key_info.W.is_held())
    {
      camera.translation.y() += 0.1;
    }

    if (app_state.key_info.S.is_held())
    {
      camera.translation.y() -= 0.1;
    }

    if (app_state.key_info.A.is_held())
    {
      camera.translation.x() += 0.1;
    }

    if (app_state.key_info.D.is_held())
    {
      camera.translation.x() -= 0.1;
    }

    primitives_renderer->draw(graphics::to_camera_matrix(camera, app_state.window_size.cast<float>()), registry);

    ImGui::SetNextWindowPos(ImVec2{0, 0});
    ImGui::Begin("editor", nullptr, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar));

    const auto available_space = ImGui::GetContentRegionAvail();

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::MenuItem("open"))
      {
        ImGuiFileDialog::Instance()->OpenDialog("AssetPicker", "Choose File", ".png,.jpg,.txt", ".");
      }
      ImGui::EndMenuBar();
    }

    // display
    if (ImGuiFileDialog::Instance()->Display("AssetPicker"))
    {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        const std::filesystem::path file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
        if (const auto id_or_error = core::resource::create(registry, file_path_name); !id_or_error.has_value())
        {
          std::cerr << id_or_error.error() << std::endl;
        }
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Image(
      reinterpret_cast<void*>(rtt->texture().get_id()),
      ImVec2(rtt->texture().shape().height, rtt->texture().shape().width),
      {0, 1},
      {1, 0});

    ImGui::Text("%s", "textures");
    registry.view<core::resource::Texture::Tag, core::resource::Path, device::Texture, TextureDisplayProperties>().each(
      [available_space,
       &registry](const entt::entity guid, const auto& path, const auto& texture, auto& texture_display_properties) {
        ImGui::PushID(path.string().c_str());
        const bool should_delete = ImGui::Button("delete");
        ImGui::SameLine();
        ImGui::Text("%s", path.string().c_str());

        ImGui::SliderFloat(
          "zoom",
          &texture_display_properties.zoom,
          TextureDisplayProperties::kMinZoom,
          TextureDisplayProperties::kMaxZoom);

        ImGui::Text("guid: %d", static_cast<int>(guid));
        ImGui::Text("size: %d x %d", texture.shape().height, texture.shape().width);
        if (should_delete)
        {
          core::resource::release(registry, path);
        }
        else
        {
          const float aspect_ratio =
            static_cast<float>(texture.shape().height) / static_cast<float>(texture.shape().width);
          const float display_height = available_space.x * texture_display_properties.zoom;
          const float display_width = aspect_ratio * display_height;

          constexpr bool kShowBorders = true;
          constexpr float kMaxDisplayHeight = 400.f;

          ImGui::BeginChild(
            path.string().c_str(),
            ImVec2{available_space.x, std::min(kMaxDisplayHeight, display_height)},
            kShowBorders,
            ImGuiWindowFlags_HorizontalScrollbar);
          {
            ImGui::Image(reinterpret_cast<void*>(texture.get_id()), ImVec2(display_width, display_height));
          }
          ImGui::EndChild();
        }
        ImGui::PopID();
      });

    ImGui::End();

    return true;
  };

  while (app->update(update_callback))
  {}
  return 0;
}
