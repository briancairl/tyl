// C++ Standard Library
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

// OpenGL
#include <GL/gl3w.h>  // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/image.hpp>

/**
 * @brief Logging callback for when glfw shits a brick
 */
static void glfw_error_callback(int error, const char* description) {}

/**
 * @brief Custom ImGui style configuration, setup on initialization
 *
 *        Note that title-bar colors match window colors, intentionally, to prevent "highlighting" when windows
 *        are hovered/focused. This is because window focusing is used to make sure time-lines are draw over all
 *        plots
 */
static void InitStyle(ImGuiStyle* style)
{
  ImVec4* colors = style->Colors;

  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
  colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
  colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
  colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
  colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

int main(int argc, char** argv)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

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

  // Create window with graphics context
  int display_w = 720, display_h = 720;
  GLFWwindow* window = glfwCreateWindow(display_w, display_h, "tyl", NULL, NULL);
  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  if (gl3wInit() != 0)
  {
    std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImGuiIO& imgui_io = ImGui::GetIO();
  // ImGuiStyle& imgui_style = ImGui::GetStyle();

  // Setup Dear ImGui style
  InitStyle(&ImGui::GetStyle());

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ImVec4 bg_color{0.1f, 0.1f, 0.1f, 1.0f};

  std::vector<tyl::graphics::Texture> loaded_textures;
  std::vector<std::string> loaded_texture_filenames;
  std::vector<std::tuple<unsigned, unsigned>> loaded_texture_dimensions;
  float texture_preview_width = 100.f;
  std::optional<unsigned> texture_selected{std::nullopt};

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("textures");
    {
      static char image_filename_buf_s[256] = "";
      if (ImGui::InputText("image file", image_filename_buf_s, sizeof(image_filename_buf_s), ImGuiInputTextFlags_EnterReturnsTrue))
      {
        tyl::graphics::Image new_image{tyl::graphics::Image::load_from_file(image_filename_buf_s)};
        tyl::graphics::Texture new_texture{new_image};

        loaded_textures.emplace_back(std::move(new_texture));
        loaded_texture_filenames.emplace_back(image_filename_buf_s);
        loaded_texture_dimensions.emplace_back(new_image.rows(), new_image.cols());
      }

      const ImGuiWindowFlags flags{
        ImGuiWindowFlags_HorizontalScrollbar |
        (ImGuiWindowFlags_NoScrollWithMouse * static_cast<bool>(texture_selected))};

      ImGui::BeginChild("texture previews", ImVec2(0, 0), /*borders*/true, flags);
      {
        // Scale preview images when a texture is selected
        if (texture_selected and ImGui::IsWindowHovered())
        {
          const float d = ImGui::GetIO().MouseWheel;
          texture_preview_width += (d * 10.f);
          texture_preview_width = std::max(100.f, std::min(texture_preview_width, 1000.f));
        }

        auto * const drawlist_p = ImGui::GetWindowDrawList();
        auto texture_itr = loaded_textures.begin();
        auto texture_fn_itr = loaded_texture_filenames.begin();
        auto texture_dim_itr = loaded_texture_dimensions.begin();
        while (texture_itr != loaded_textures.end())
        {
          const auto [rows, cols] = *texture_dim_itr;
          const float ratio = static_cast<float>(cols) / static_cast<float>(rows);

          ImGui::Image(reinterpret_cast<void*>(texture_itr->get_id()), ImVec2{texture_preview_width, texture_preview_width * ratio});

          if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
          {
            if (ImGui::IsItemHovered())
            {
              texture_selected = texture_itr->get_id();
            }
            else
            {
              texture_selected.reset();
            }
          }

          if (ImGui::IsItemHovered())
          {
            const ImVec2 min_pos = ImGui::GetItemRectMin();
            const ImVec2 max_pos = ImGui::GetItemRectMax();
            drawlist_p->AddRect(min_pos, max_pos, IM_COL32(255, 0, 255, 100), 0.0f, ImDrawCornerFlags_All, 10.f);
          }
          else if (texture_selected and texture_itr->get_id() == texture_selected.value())
          {
            const ImVec2 min_pos = ImGui::GetItemRectMin();
            const ImVec2 max_pos = ImGui::GetItemRectMax();
            drawlist_p->AddRect(min_pos, max_pos, IM_COL32(255, 255, 0, 100), 0.0f, ImDrawCornerFlags_All, 10.f);
          }

          ImGui::TextUnformatted(texture_fn_itr->c_str());
          ImGui::SameLine();
          if (ImGui::Button("x"))
          {
            texture_itr = loaded_textures.erase(texture_itr);
            texture_fn_itr = loaded_texture_filenames.erase(texture_fn_itr);
            texture_dim_itr = loaded_texture_dimensions.erase(texture_dim_itr);
          }
          else
          {
            ++texture_itr;
            ++texture_fn_itr;
            ++texture_dim_itr;
          }
        }
        ImGui::EndChild();
      }

      ImGui::End();
    }


    // ImGui::ShowStyleEditor(&imgui_style);
    // ImGui::ShowDemoWindow();
  
    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
