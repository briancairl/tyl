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


struct TileAtlasData
{
  tyl::filesystem::path file_path;
  tyl::graphics::Texture texture;
  int height_px;
  int width_px;
  int tile_size_px;
  int top_trim_px;
  int left_trim_px;

  explicit TileAtlasData(tyl::filesystem::path _file_path) :
      file_path{std::move(_file_path)},
      texture{[this]() -> tyl::graphics::Image {
        auto image = tyl::graphics::Image::load_from_file(file_path.c_str());
        this->height_px = image.rows();
        this->width_px = image.cols();
        return image;
      }()},
      tile_size_px{16},
      top_trim_px{0},
      left_trim_px{0}
  {}

  inline int tile_grid_height() const { return (this->height_px - this->top_trim_px) / this->tile_size_px; }

  inline int tile_grid_width() const { return (this->width_px - this->left_trim_px) / this->tile_size_px; }
};

enum class Step
{
  SELECT_LOAD_DATA,
  SELECT_TILE_SIZING,
  INITIALIZE_MAP,
  EDIT_MAP
};

struct MapData
{
  static constexpr int NO_TILE = -1;

  inline void resize(int height, int width, int layer_count)
  {
    this->layers.resize(layer_count);
    this->layer_labels.resize(layer_count);
    this->layer_visibility.resize(layer_count, true);

    for (std::size_t i = 0; i < this->layer_labels.size(); ++i)
    {
      std::ostringstream oss;
      oss << "layer #" << i;
      this->layer_labels[i] = oss.str();
    }

    for (auto& layer : this->layers)
    {
      layer.resize(height * width, NO_TILE);
    }
  }

  inline bool empty() const { return layers.empty(); }

  inline void clear()
  {
    for (auto& layer : this->layers)
    {
      std::fill(layer.begin(), layer.end(), NO_TILE);
    }
  }

  std::vector<std::vector<int>> layers;
  std::vector<std::string> layer_labels;
  std::vector<std::uint8_t> layer_visibility;
};


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

  static constexpr auto dialogue_options = tyl::ui::FileDialogue::Options::NoMultiSelect |
    tyl::ui::FileDialogue::Options::FileMustExist | tyl::ui::FileDialogue::Options::AllowSelectRegularFile;

  tyl::ui::FileDialogue dialogue{
    "open", "png|jpe?g|tyl", dialogue_options, tyl::filesystem::path{"/home/brian/Desktop"}};

  Step editor_step{Step::SELECT_LOAD_DATA};

  std::optional<TileAtlasData> loaded_atlas;

  float zoom_factor = 4.f;

  static constexpr int MAP_LAYER_COUNT_MAX = 10;
  static constexpr int MAP_HEIGHT_MAX = 5000;
  static constexpr int MAP_WIDTH_MAX = 5000;

  int map_layer_count = 1;
  int map_height = 100;
  int map_width = 100;

  int selected_tile_id = -1;
  int selected_edit_map_layer = 0;

  MapData map_data;

  ImColor selection_fill_color{1.f, 1.f, 0.f, 0.8f};
  ImColor selection_line_color{1.f, 0.f, 0.f, 0.8f};
  ImColor grid_line_color{0.9f, 0.9f, 0.9f, 0.9f};

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // clang-format off
    const auto edittor_window_options =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse |
      ImGuiWindowFlags_MenuBar;
    // clang-format on

    ImGui::SetNextWindowSize(ImVec2(display_w, display_h));
    ImGui::SetNextWindowPos(ImVec2{0.f, 0.f});
    {
      ImGui::Begin("##edittor", nullptr, edittor_window_options);

      if (ImGui::BeginMenuBar())
      {
        if (ImGui::BeginMenu("file"))
        {
          if (ImGui::MenuItem("new file"))
          {
            editor_step = Step::SELECT_LOAD_DATA;
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("edit", editor_step == Step::EDIT_MAP))
        {
          if (ImGui::MenuItem("clear all"))
          {
            map_data.clear();
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("options"))
        {
          if (ImGui::BeginMenu("app theme"))
          {
            if (ImGui::MenuItem("dark"))
              ImGui::StyleColorsDark();
            if (ImGui::MenuItem("light"))
              ImGui::StyleColorsLight();
            if (ImGui::MenuItem("classic"))
              ImGui::StyleColorsClassic();
            ImGui::EndMenu();
          }

          if (ImGui::BeginMenu("colors", editor_step == Step::EDIT_MAP or editor_step == Step::SELECT_TILE_SIZING))
          {
            if (ImGui::BeginMenu("grid line color"))
            {
              ImGui::ColorPicker4(
                "color", reinterpret_cast<float*>(&grid_line_color), ImGuiColorEditFlags_NoSmallPreview);
              ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("selection fill color"))
            {
              ImGui::ColorPicker4(
                "color", reinterpret_cast<float*>(&selection_fill_color), ImGuiColorEditFlags_NoSmallPreview);
              ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("selection line color"))
            {
              ImGui::ColorPicker4(
                "color", reinterpret_cast<float*>(&selection_line_color), ImGuiColorEditFlags_NoSmallPreview);
              ImGui::EndMenu();
            }

            ImGui::EndMenu();
          }

          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("view"))
        {
          if (ImGui::BeginMenu("zoom"))
          {
            ImGui::SliderFloat("zoom", &zoom_factor, 0.1f, 10.f);
            ImGui::EndMenu();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      switch (editor_step)
      {
      case Step::SELECT_LOAD_DATA:
      {
        if (tyl::ui::FileDialogue::UpdateStatus::Selected == dialogue.update())
        {
          for (const auto& file : dialogue)
          {
            // Load previous session data
            if (file.extension() == ".tyl")
            {
            }
            // Load new texture for new session
            else
            {
              try
              {
                loaded_atlas.emplace(file);
                editor_step = Step::SELECT_TILE_SIZING;
              }
              catch (const std::runtime_error& err)
              {
                // FAILED TO LOAD IMAGE
              }
            }
            break;
          }
        }
        break;
      }
      case Step::SELECT_TILE_SIZING:
      {
        static constexpr float button_width = 50.f;

        // Button to go back to texture selection dialogue
        if (ImGui::Button("back", ImVec2{button_width, 0.f}))
        {
          editor_step = Step::SELECT_LOAD_DATA;
        }
        if (ImGui::IsItemHovered())
        {
          ImGui::SetTooltip("go back to file selection dialogue");
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2{ImGui::GetWindowContentRegionWidth() - button_width, 0.f} - ImGui::GetCursorPos());
        ImGui::SameLine();

        // Button to lock-in tile size and proceed to main editor
        if (ImGui::Button("next", ImVec2{button_width, 0.f}))
        {
          editor_step = Step::INITIALIZE_MAP;
        }
        if (ImGui::IsItemHovered())
        {
          ImGui::SetTooltip("finalize tile size settings and proceed to editor");
        }

        // Show the loaded texture with tile division layout
        if (ImGui::BeginTable("##tile-settings-table", 2, ImGuiTableFlags_Resizable))
        {
          ImGui::TableSetupColumn("##texture-preview", ImGuiTableColumnFlags_WidthStretch, 0.75f);
          ImGui::TableSetupColumn("##tile-properties", ImGuiTableColumnFlags_WidthStretch, 0.25f);
          ImGui::TableHeadersRow();

          ImGui::TableNextColumn();
          {
            const float image_width = loaded_atlas->width_px * zoom_factor;
            const float image_height = loaded_atlas->height_px * zoom_factor;
            const float tile_size = loaded_atlas->tile_size_px * zoom_factor;
            const float image_centering_offset = std::max(0.f, (ImGui::GetContentRegionAvail().x - image_width) * 0.5f);

            // clang-format off
            ImGui::BeginChild(
              loaded_atlas->file_path.filename().c_str(),
              ImVec2{0.f, 0.f},
              true, /*borders*/
              ImGuiWindowFlags_HorizontalScrollbar
            );
            // clang-format on

            // clang-format off
            const ImVec2 origin =
              ImGui::GetWindowPos() +
              ImGui::GetCursorPos() -
              ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()} +
              ImVec2(loaded_atlas->left_trim_px + image_centering_offset, loaded_atlas->top_trim_px);
            // clang-format on

            ImGui::SetCursorPos(ImVec2{image_centering_offset, 0.f} + ImGui::GetCursorPos());
            ImGui::Image(reinterpret_cast<void*>(loaded_atlas->texture.get_id()), ImVec2{image_width, image_height});

            auto* const drawlist = ImGui::GetWindowDrawList();

            const float line_height = loaded_atlas->tile_grid_height() * tile_size;
            for (int i = 0; i <= loaded_atlas->tile_grid_width(); i++)
            {
              drawlist->AddLine(
                origin + ImVec2{i * tile_size, 0.f}, origin + ImVec2{i * tile_size, line_height}, grid_line_color);
            }

            const float line_width = loaded_atlas->tile_grid_width() * tile_size;
            for (int j = 0; j <= loaded_atlas->tile_grid_height(); j++)
            {
              drawlist->AddLine(
                origin + ImVec2{0.f, j * tile_size}, origin + ImVec2{line_width, j * tile_size}, grid_line_color);
            }

            ImGui::EndChild();
          }

          ImGui::TableNextColumn();
          {
            {
              ImGui::BeginChild(
                "texture-info",
                ImVec2{0.f, ImGui::GetTextLineHeightWithSpacing() * 4.f},
                true, /*borders*/
                ImGuiWindowFlags_NoScrollbar);

              ImGui::Text("%s", loaded_atlas->file_path.c_str());
              ImGui::Text("%d x %d px", loaded_atlas->height_px, loaded_atlas->width_px);
              ImGui::Text("%d x %d tiles", loaded_atlas->tile_grid_height(), loaded_atlas->tile_grid_width());

              ImGui::EndChild();
            }
            {
              ImGui::BeginChild(
                "tile-property-selections",
                ImVec2{0.f, ImGui::GetTextLineHeightWithSpacing() * 5.f},
                true, /*borders*/
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
              ImGui::SliderInt("tile size", &loaded_atlas->tile_size_px, 2, 128);
              ImGui::SliderInt("top trim", &loaded_atlas->top_trim_px, 0, loaded_atlas->height_px);
              ImGui::SliderInt("left trim", &loaded_atlas->left_trim_px, 0, loaded_atlas->width_px);
              ImGui::EndChild();
            }
            {
              ImGui::BeginChild(
                "map-property-selections",
                ImVec2{0.f, ImGui::GetTextLineHeightWithSpacing() * 5.f},
                true, /*borders*/
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
              ImGui::SliderInt("map layer count", &map_layer_count, 1, MAP_LAYER_COUNT_MAX);
              ImGui::SliderInt("map height", &map_height, 1, MAP_HEIGHT_MAX);
              ImGui::SliderInt("map width", &map_width, 1, MAP_WIDTH_MAX);

              ImGui::EndChild();
            }
          }
          ImGui::EndTable();
        }
        break;
      }
      case Step::INITIALIZE_MAP:
      {
        ImGui::TextUnformatted("initializing map data...");
        editor_step = Step::EDIT_MAP;
        break;
      }
      case Step::EDIT_MAP:
      {
        if (map_data.empty())
        {
          map_data.resize(map_height, map_width, map_layer_count);
        }

        if (ImGui::BeginTable("##main-edittor-table", 2, ImGuiTableFlags_Resizable))
        {
          const float available_column_height = ImGui::GetContentRegionAvail().y;

          ImGui::TableNextColumn();
          {
            // clang-format off
            ImGui::BeginChild(
              "##tile-selector",
              ImVec2{0.f, available_column_height * 0.75f},
              true, /*borders*/
              ImGuiWindowFlags_HorizontalScrollbar
            );
            // clang-format on

            const float image_width = loaded_atlas->width_px * zoom_factor;
            const float image_height = loaded_atlas->height_px * zoom_factor;
            const float tile_size = loaded_atlas->tile_size_px * zoom_factor;
            const float image_centering_offset = std::max(0.f, (ImGui::GetContentRegionAvail().x - image_width) * 0.5f);

            // clang-format off
            const ImVec2 origin =
              ImGui::GetWindowPos() +
              ImGui::GetCursorPos() -
              ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()} +
              ImVec2(loaded_atlas->left_trim_px + image_centering_offset, loaded_atlas->top_trim_px);
            // clang-format on

            ImGui::SetCursorPos(ImVec2{image_centering_offset, 0.f} + ImGui::GetCursorPos());
            ImGui::Image(reinterpret_cast<void*>(loaded_atlas->texture.get_id()), ImVec2{image_width, image_height});

            auto* const drawlist = ImGui::GetWindowDrawList();

            const int grid_height = loaded_atlas->tile_grid_height();
            const int grid_width = loaded_atlas->tile_grid_width();

            const float line_height = grid_height * tile_size;
            for (int i = 0; i <= grid_width; i++)
            {
              drawlist->AddLine(
                origin + ImVec2{i * tile_size, 0.f}, origin + ImVec2{i * tile_size, line_height}, grid_line_color);
            }

            const float line_width = grid_width * tile_size;
            for (int j = 0; j <= grid_height; j++)
            {
              drawlist->AddLine(
                origin + ImVec2{0.f, j * tile_size}, origin + ImVec2{line_width, j * tile_size}, grid_line_color);
            }

            for (int i = 0; i < grid_height; i++)
            {
              for (int j = 0; j < grid_width; j++)
              {
                const ImVec2 top_left{origin + ImVec2((j + 0) * tile_size, (i + 0) * tile_size)};
                const ImVec2 bottom_right{origin + ImVec2((j + 1) * tile_size, (i + 1) * tile_size)};

                if ((i * grid_width + j) == selected_tile_id)
                {
                  drawlist->AddRectFilled(top_left, bottom_right, selection_fill_color);
                  drawlist->AddRect(top_left, bottom_right, selection_line_color);
                }
                else if (ImGui::IsMouseHoveringRect(top_left, bottom_right))
                {
                  drawlist->AddRectFilled(top_left, bottom_right, selection_fill_color);
                  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                  {
                    selected_tile_id = i * grid_width + j;
                  }
                }
              }
            }

            ImGui::EndChild();
          }

          {
            // clang-format off
            ImGui::BeginChild(
              "##layer-selector",
              ImVec2{0.f, available_column_height * 0.25f},
              true, /*borders*/
              ImGuiWindowFlags_HorizontalScrollbar
            );
            // clang-format on

            if (ImGui::BeginTable("##layer-selector-table", 2, ImGuiTableFlags_Resizable))
            {
              {
                ImGui::TableSetupColumn("editting", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableSetupColumn("visibility", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableHeadersRow();
              }

              for (int l = 0; l < static_cast<int>(map_data.layers.size()); ++l)
              {
                ImGui::TableNextColumn();
                ImGui::PushID(l);
                if (ImGui::RadioButton(map_data.layer_labels[l].c_str(), selected_edit_map_layer == l))
                {
                  selected_edit_map_layer = l;
                }

                ImGui::TableNextColumn();
                if (ImGui::RadioButton("visible", map_data.layer_visibility[l]))
                {
                  map_data.layer_visibility[l] = !map_data.layer_visibility[l];
                }
              }

              for (int l = 0; l < static_cast<int>(map_data.layers.size()); ++l)
              {
                ImGui::PopID();
              }

              ImGui::EndTable();
            }

            ImGui::EndChild();
          }

          ImGui::TableNextColumn();
          {
            ImGui::BeginChild("##map-editor", ImVec2{0.f, 0.f}, true /*borders*/, ImGuiWindowFlags_HorizontalScrollbar);

            // clang-format off
            const ImVec2 origin =
              ImGui::GetWindowPos() +
              ImGui::GetCursorPos() -
              ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()} +
              ImVec2(loaded_atlas->left_trim_px, loaded_atlas->top_trim_px);
            // clang-format on

            const int atlas_grid_height = loaded_atlas->tile_grid_height();
            const int atlas_grid_width = loaded_atlas->tile_grid_width();
            const float tile_size = loaded_atlas->tile_size_px * zoom_factor;

            ImGui::InvisibleButton("##nav-deadzone", ImVec2{map_width * tile_size, map_height * tile_size});

            auto* const drawlist = ImGui::GetWindowDrawList();
            const float line_height = map_height * tile_size;
            const float line_width = map_width * tile_size;

            for (std::size_t l = 0; l != map_data.layers.size(); ++l)
            {
              if (!map_data.layer_visibility[l])
              {
                continue;
              }

              auto& layer = map_data.layers[l];

              for (int i = 0; i < map_height; i++)
              {
                for (int j = 0; j < map_width; j++)
                {
                  const ImVec2 top_left{origin + ImVec2((j + 0) * tile_size, (i + 0) * tile_size)};
                  if (const int id = layer[i * map_width + j]; id >= 0)
                  {
                    ImGui::SetCursorScreenPos(top_left);
                    const int x = id / atlas_grid_width;
                    const int y = id % atlas_grid_width;
                    const float v0 = (1.0f / atlas_grid_height) * (x + 0);
                    const float v1 = (1.0f / atlas_grid_height) * (x + 1);
                    const float u0 = (1.0f / atlas_grid_width) * (y + 0);
                    const float u1 = (1.0f / atlas_grid_width) * (y + 1);
                    ImGui::Image(
                      reinterpret_cast<void*>(loaded_atlas->texture.get_id()),
                      ImVec2{tile_size, tile_size},
                      ImVec2{u0, v0},
                      ImVec2{u1, v1});
                  }
                }
              }
            }

            {
              auto& layer = map_data.layers[selected_edit_map_layer];
              for (int i = 0; i < map_height; i++)
              {
                for (int j = 0; j < map_width; j++)
                {
                  const ImVec2 top_left{origin + ImVec2((j + 0) * tile_size, (i + 0) * tile_size)};
                  const ImVec2 bottom_right{origin + ImVec2((j + 1) * tile_size, (i + 1) * tile_size)};
                  if (ImGui::IsMouseHoveringRect(top_left, bottom_right))
                  {
                    drawlist->AddRectFilled(top_left, bottom_right, selection_fill_color);
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                    {
                      layer[i * map_width + j] = MapData::NO_TILE;
                    }
                    else if (selected_tile_id != MapData::NO_TILE)
                    {
                      const int current_id = layer[i * map_width + j];
                      if (current_id == MapData::NO_TILE and ImGui::IsMouseDown(ImGuiMouseButton_Left))
                      {
                        layer[i * map_width + j] = selected_tile_id;
                      }
                      else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                      {
                        layer[i * map_width + j] = selected_tile_id;
                      }
                    }
                  }
                }
              }
            }

            for (int i = 0; i <= map_width; i++)
            {
              drawlist->AddLine(
                origin + ImVec2{i * tile_size, 0.f}, origin + ImVec2{i * tile_size, line_height}, grid_line_color);
            }

            for (int j = 0; j <= map_height; j++)
            {
              drawlist->AddLine(
                origin + ImVec2{0.f, j * tile_size}, origin + ImVec2{line_width, j * tile_size}, grid_line_color);
            }

            ImGui::EndChild();
          }

          ImGui::EndTable();
        }
        break;
      }
      default:
      {
        break;
      }
      }  // switch (editor_step)
      ImGui::End();
    }

    // ImGui::ShowStyleEditor(&imgui_style);
    // ImGui::ShowDemoWindow();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
