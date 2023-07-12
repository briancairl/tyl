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

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/core/engine/resource.hpp>
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/render_target_texture.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/engine/primitives_renderer.hpp>
#include <tyl/graphics/engine/types.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/utility/expected.hpp>

using namespace tyl::engine;
using namespace tyl::graphics;

static void glfw_error_callback(int error, const char* description)
{
  std::fprintf(stderr, "%d : %s\n", error, description);
}

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
  entt::locator<core::resource::Texture::Locator>::emplace<DefaultTextureLocator>();

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit())
  {
    std::terminate();
  }

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

  int x_size = 2000;
  int y_size = 1000;

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(x_size, y_size, "editor", NULL, NULL);

  TYL_ASSERT_NON_NULL(window);

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::terminate();
  }
  glfwSwapInterval(1);  // Enable vsync

  // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

  device::enable_debug_logs();
  device::enable_error_logs();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  entt::registry registry;
  auto primitives_renderer = graphics::PrimitivesRenderer::create({.max_vertex_count = 100});

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


  if (!primitives_renderer.has_value())
  {
    return 1;
  }

  auto rtt = device::RenderTargetTexture::create(500, 500);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    rtt->draw_to([&primitives_renderer, &registry](const int viewport_height, const int viewport_width) {
      primitives_renderer->update(registry);
    });

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

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
      reinterpret_cast<void*>(rtt->texture().get_id()), ImVec2(rtt->texture().height(), rtt->texture().width()));

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
        ImGui::Text("size: %d x %d", texture.height(), texture.width());
        if (should_delete)
        {
          core::resource::release(registry, path);
        }
        else
        {
          const float aspect_ratio = static_cast<float>(texture.height()) / static_cast<float>(texture.width());
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }


  return 0;
}
