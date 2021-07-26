// C++ Standard Library
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_stdlib.h>

// OpenGL
#include <GL/gl3w.h>  // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/ui/file_dialogue.hpp>

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

  tyl::ui::FileDialogue dialogue{"open",
                                 "png|jpe?g",
                                 tyl::ui::FileDialogue::Options::FileMustExist |
                                   tyl::ui::FileDialogue::Options::AllowSelectRegularFile,
                                 tyl::filesystem::path{"/home/brian/Desktop"}};

  struct TextureData
  {
    tyl::filesystem::path file_path;
    tyl::graphics::Texture texture;
    int height_px;
    int width_px;
    int cell_size_px;
    int top_trim_px;
    int left_trim_px;
    float zoom_factor;
    bool locked;

    explicit TextureData(tyl::filesystem::path _file_path) :
        file_path{std::move(_file_path)},
        texture{[this]() -> tyl::graphics::Image {
          auto image = tyl::graphics::Image::load_from_file(file_path.c_str());
          this->height_px = image.rows();
          this->width_px = image.cols();
          return image;
        }()},
        cell_size_px{16},
        top_trim_px{0},
        left_trim_px{0},
        zoom_factor{1.f},
        locked{false}
    {}
  };

  std::vector<TextureData> loaded_textures;
  std::optional<tyl::filesystem::path> selected_texture_path;
  auto selected_texture_itr = loaded_textures.end();
  int selected_tile_id = -1;

  int map_height = 100;
  int map_width = 100;
  std::vector<int> map_data;
  map_data.resize(map_height * map_width, -1);

  ImColor selection_color{1.f, 1.f, 0.f, 0.8f};
  ImColor grid_line_color{1.f, 0.f, 0.f, 0.8f};
  float* color_editting = nullptr;

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool refresh_selected_texture = false;

    // Displays texture source file browser
    ImGui::Begin("texture source browser");
    if (tyl::ui::FileDialogue::UpdateStatus::Selected == dialogue.update())
    {
      for (const auto& file : dialogue)
      {
        if (std::find_if(loaded_textures.begin(), loaded_textures.end(), [&file](const auto& tex_data) {
              return tex_data.file_path == file;
            }) == loaded_textures.end())
        {
          loaded_textures.emplace_back(file);
          refresh_selected_texture = true;
        }
      }
    }
    ImGui::End();

    // Previews texture sheets which have alread been load
    {
      ImGui::Begin("loaded textures");

      // Handle loaded texture preview list interactions
      auto removing_texture_itr = loaded_textures.end();
      for (auto texture_itr = loaded_textures.begin(); texture_itr != loaded_textures.end(); ++texture_itr)
      {
        const float width = 100.f;
        const float height_to_width =
          static_cast<float>(texture_itr->height_px) / static_cast<float>(texture_itr->width_px);

        ImGui::TextUnformatted("click to remove");
        ImGui::SameLine();
        if (ImGui::Button(texture_itr->file_path.filename().c_str()))
        {
          removing_texture_itr = texture_itr;
        }
        ImGui::BeginChild(
          texture_itr->file_path.filename().c_str(), ImVec2{0.f, 200.f}, true /*borders*/
        );
        ImGui::Image(reinterpret_cast<void*>(texture_itr->texture.get_id()), ImVec2{width, height_to_width * width});
        ImGui::EndChild();

        if (ImGui::IsItemHovered())
        {
          auto* const child_draw_list = ImGui::GetWindowDrawList();
          child_draw_list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), selection_color);

          if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
          {
            selected_texture_path.emplace(texture_itr->file_path);
            refresh_selected_texture = true;
          }
        }
      }

      // Handle texture unloading
      if (removing_texture_itr != loaded_textures.end())
      {
        loaded_textures.erase(removing_texture_itr);
        refresh_selected_texture = true;
      }

      ImGui::End();
    }

    // Handle selected texture updates
    if (refresh_selected_texture)
    {
      if (selected_texture_path)
      {
        selected_texture_itr = std::find_if(
          loaded_textures.begin(), loaded_textures.end(), [&selected_texture_path](const auto& texture_data) {
            return texture_data.file_path == *selected_texture_path;
          });
      }
      else
      {
        selected_texture_itr = loaded_textures.end();
      }
    }


    // Adjacency Editor
    ImGui::Begin("adjacency editor");
    ImGui::PushItemWidth(200.f);
    ImGui::ColorEdit4(
      "cell selection color", reinterpret_cast<float*>(&selection_color), ImGuiColorEditFlags_NoSmallPreview);
    if (
      ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonLeft) and
      ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft))
    {
      ImGui::OpenPopup("color picker");
      color_editting = reinterpret_cast<float*>(&selection_color);
    }

    ImGui::ColorEdit4("grid color", reinterpret_cast<float*>(&grid_line_color), ImGuiColorEditFlags_NoSmallPreview);
    if (
      ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonLeft) and
      ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft))
    {
      ImGui::OpenPopup("color picker");
      color_editting = reinterpret_cast<float*>(&grid_line_color);
    }

    if (ImGui::BeginPopupModal("color picker"))
    {
      if (ImGui::Button("close"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::ColorPicker4("cell selection color", color_editting, ImGuiColorEditFlags_NoSmallPreview);
      ImGui::EndPopup();
    }
    else
    {
      color_editting = nullptr;
    }
    ImGui::PopItemWidth();

    if (selected_texture_itr != loaded_textures.end())
    {
      const float width = selected_texture_itr->width_px * selected_texture_itr->zoom_factor;
      const float height = selected_texture_itr->height_px * selected_texture_itr->zoom_factor;
      const float cell_size = selected_texture_itr->cell_size_px * selected_texture_itr->zoom_factor;

      const int texture_width_c =
        (selected_texture_itr->width_px - selected_texture_itr->left_trim_px) / selected_texture_itr->cell_size_px;
      const int texture_height_c =
        (selected_texture_itr->height_px - selected_texture_itr->top_trim_px) / selected_texture_itr->cell_size_px;

      if (ImGui::BeginTable("##painting-sections", 2, ImGuiTableFlags_Resizable))
      {
        // Tile selector
        {
          ImGui::TableNextColumn();

          if (!selected_texture_itr->locked)
          {
            ImGui::SliderInt("cell size", &selected_texture_itr->cell_size_px, 2, 128);
            ImGui::SliderInt("top trim", &selected_texture_itr->top_trim_px, 0, selected_texture_itr->height_px);
            ImGui::SliderInt("left trim", &selected_texture_itr->left_trim_px, 0, selected_texture_itr->width_px);
            selected_texture_itr->locked = ImGui::Button("lock");
          }
          else
          {
            selected_texture_itr->locked = !ImGui::Button("unlock");
          }

          {
            ImGui::BeginChild(
              "##tile-selector", ImVec2{0.f, 0.f}, true /*borders*/, ImGuiWindowFlags_HorizontalScrollbar);

            {
              const ImVec2 origin = ImGui::GetWindowPos() + ImGui::GetCursorPos() -
                ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()} +
                ImVec2(selected_texture_itr->left_trim_px, selected_texture_itr->top_trim_px);

              ImGui::Image(reinterpret_cast<void*>(selected_texture_itr->texture.get_id()), ImVec2{width, height});

              auto* const drawlist = ImGui::GetWindowDrawList();
              const float line_height = texture_height_c * cell_size;
              const float line_width = texture_width_c * cell_size;

              for (int i = 0; i <= texture_width_c; i++)
              {
                drawlist->AddLine(
                  origin + ImVec2{i * cell_size, 0.f}, origin + ImVec2{i * cell_size, line_height}, grid_line_color);
              }

              for (int j = 0; j <= texture_height_c; j++)
              {
                drawlist->AddLine(
                  origin + ImVec2{0.f, j * cell_size}, origin + ImVec2{line_width, j * cell_size}, grid_line_color);
              }

              for (int i = 0; i < texture_height_c; i++)
              {
                for (int j = 0; j < texture_width_c; j++)
                {
                  const ImVec2 top_left{origin + ImVec2((j + 0) * cell_size, (i + 0) * cell_size)};
                  const ImVec2 bottom_right{origin + ImVec2((j + 1) * cell_size, (i + 1) * cell_size)};

                  if ((i * texture_width_c + j) == selected_tile_id)
                  {
                    drawlist->AddRectFilled(top_left, bottom_right, selection_color);
                  }

                  if (ImGui::IsMouseHoveringRect(top_left, bottom_right))
                  {
                    drawlist->AddRectFilled(top_left, bottom_right, selection_color);
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                      selected_tile_id = i * texture_width_c + j;
                    }
                  }
                }
              }
            }

            ImGui::EndChild();
          }
        }

        // Tile painter
        {
          ImGui::TableNextColumn();
          ImGui::SliderFloat("zoom", &selected_texture_itr->zoom_factor, 1.f, 10.f);

          bool resize_map = false;
          resize_map |= ImGui::SliderInt("map height", &map_height, 20, 2000);
          resize_map |= ImGui::SliderInt("map width", &map_width, 20, 2000);

          if (resize_map)
          {
            map_data.resize(map_height * map_width, -1);
          }

          {
            ImGui::BeginChild(
              "##tile-painter", ImVec2{0.f, 0.f}, true /*borders*/, ImGuiWindowFlags_HorizontalScrollbar);

            const ImVec2 origin = ImGui::GetWindowPos() + ImGui::GetCursorPos() -
              ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()} +
              ImVec2(selected_texture_itr->left_trim_px, selected_texture_itr->top_trim_px);

            ImGui::InvisibleButton("##nav-deadzone", ImVec2{map_width * cell_size, map_height * cell_size});

            auto* const drawlist = ImGui::GetWindowDrawList();
            const float line_height = map_height * cell_size;
            const float line_width = map_width * cell_size;

            for (int i = 0; i < map_height; i++)
            {
              for (int j = 0; j < map_width; j++)
              {
                const ImVec2 top_left{origin + ImVec2((j + 0) * cell_size, (i + 0) * cell_size)};
                const ImVec2 bottom_right{origin + ImVec2((j + 1) * cell_size, (i + 1) * cell_size)};

                if (const int id = map_data[i * map_width + j]; id >= 0)
                {
                  ImGui::SetCursorScreenPos(top_left);
                  const int x = id / texture_width_c;
                  const int y = id % texture_width_c;
                  const float v0 = (1.0f / texture_height_c) * (x + 0);
                  const float v1 = (1.0f / texture_height_c) * (x + 1);
                  const float u0 = (1.0f / texture_width_c) * (y + 0);
                  const float u1 = (1.0f / texture_width_c) * (y + 1);
                  ImGui::Image(
                    reinterpret_cast<void*>(selected_texture_itr->texture.get_id()),
                    ImVec2{cell_size, cell_size},
                    ImVec2{u0, v0},
                    ImVec2{u1, v1});
                }

                if (ImGui::IsMouseHoveringRect(top_left, bottom_right))
                {
                  drawlist->AddRectFilled(top_left, bottom_right, selection_color);
                  if (selected_tile_id > -1 and ImGui::IsMouseDown(ImGuiMouseButton_Left))
                  {
                    map_data[i * map_width + j] = selected_tile_id;
                  }
                  else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                  {
                    map_data[i * map_width + j] = -1;
                  }
                }
              }
            }

            for (int i = 0; i <= map_width; i++)
            {
              drawlist->AddLine(
                origin + ImVec2{i * cell_size, 0.f}, origin + ImVec2{i * cell_size, line_height}, grid_line_color);
            }

            for (int j = 0; j <= map_height; j++)
            {
              drawlist->AddLine(
                origin + ImVec2{0.f, j * cell_size}, origin + ImVec2{line_width, j * cell_size}, grid_line_color);
            }

            ImGui::EndChild();
          }
        }

        ImGui::EndTable();
      }
    }

    ImGui::End();

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
