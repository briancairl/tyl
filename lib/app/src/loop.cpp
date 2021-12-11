/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.cpp
 */

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// OpenGL
#include <GL/gl3w.h>  // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Art
#include <tyl/app/loop.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/logging.hpp>
#include <tyl/ui/style.hpp>

namespace tyl::app
{
namespace  // anonymous
{

/**
 * @brief Logging callback for when glfw shits a brick
 */
void glfw_error_callback(int error, const char* description)
{
  TYL_CRITICAL("[glfw_error_callback] error-code={d} msg={}", error, description);
}

/**
 * @brief Computes normalized cursor position
 */
inline Vec2f to_cursor_position_normalized(const graphics::Target& render_target, const WindowState& window_state)
{
  const float xn = window_state.cursor_position_full_resolution.x() / render_target.viewport_size.x();
  const float yn = window_state.cursor_position_full_resolution.y() / render_target.viewport_size.y();
  return Vec2f{2.f * xn - 1.0f, 1.0f - 2.f * yn};
}

}  // namespace anonymous


Loop::Loop(const char* name, const Vec2i& size) : window_name_{name}, window_ctx_{nullptr}
{
  logging::initialize();

  window_render_target_.viewport_size = size;

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
  {
    TYL_CRITICAL("[{}] glfwInit failed", window_name_);
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

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(size.x(), size.y(), window_name_, NULL, NULL);

  if (window == NULL)
  {
    TYL_CRITICAL("[{}] glfwCreateWindow failed", window_name_);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

  if (gl3wInit() != 0)
  {
    TYL_CRITICAL("[{}] gl3wInit failed", window_name_);
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ui::InitStyleDefault(&ImGui::GetStyle());

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  window_ctx_ = reinterpret_cast<void*>(window);
}

Loop::~Loop()
{
  if (window_ctx_ != nullptr)
  {
    TYL_DEBUG("[{}] ImGui cleanup", window_name_);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    TYL_DEBUG("[{}] glfwDestroyWindow", window_name_);
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(window_ctx_));

    TYL_DEBUG("[{}] glfwTerminate", window_name_);
    glfwTerminate();
  }
}

int Loop::run(const std::function<bool(graphics::Target&, const WindowState&, const duration dt)>& loop_fn)
{
  GLFWwindow* window = reinterpret_cast<GLFWwindow*>(window_ctx_);

  ImVec4 background_color{0.1f, 0.1f, 0.1f, 1.0f};

  graphics::Target window_render_target;

  TYL_INFO("[{}] starting", window_name_);

  time_point prev_updater_stamp = clock::now();
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glfwGetCursorPos(
      window, &window_state_.cursor_position_full_resolution.x(), &window_state_.cursor_position_full_resolution.y());
    window_state_.cursor_position_normalized = to_cursor_position_normalized(window_render_target_, window_state_);

    glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowStyleEditor(&imgui_style);
    // ImGui::ShowDemoWindow();

    static constexpr std::array<std::pair<int, std::uint64_t>, 6UL> key_scan{
      {{GLFW_KEY_W, WindowState::MoveUp},
       {GLFW_KEY_S, WindowState::MoveDown},
       {GLFW_KEY_D, WindowState::MoveRight},
       {GLFW_KEY_A, WindowState::MoveLeft},
       {GLFW_KEY_LEFT_SHIFT, WindowState::Sprint},
       {GLFW_KEY_SPACE, WindowState::Jump}}};

    window_state_.input_down_mask = 0;
    window_state_.input_up_mask = 0;
    for (const auto& [code, mask] : key_scan)
    {
      if (int state = glfwGetKey(window, code); state == GLFW_PRESS)
      {
        window_state_.input_down_mask |= mask;
      }
      else if (state == GLFW_RELEASE)
      {
        window_state_.input_up_mask |= mask;
      }
    }
    window_state_.input_pressed_mask =
      (window_state_.input_down_mask) & (window_state_.input_down_mask ^ window_state_.previous_input_down_mask);
    window_state_.input_released_mask =
      (window_state_.input_up_mask) & (window_state_.input_up_mask ^ window_state_.previous_input_up_mask);
    window_state_.previous_input_down_mask = window_state_.input_down_mask;
    window_state_.previous_input_up_mask = window_state_.input_up_mask;

    time_point curr_updater_stamp = clock::now();
    if (!loop_fn(window_render_target_, window_state_, curr_updater_stamp - prev_updater_stamp))
    {
      break;
    }
    prev_updater_stamp = curr_updater_stamp;

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &window_render_target_.viewport_size.x(), &window_render_target_.viewport_size.y());
    glViewport(0, 0, window_render_target_.viewport_size.x(), window_render_target_.viewport_size.y());
    glfwSwapBuffers(window);
  }
  TYL_INFO("[{}] closing", window_name_);
  return 0;
}

}  // namespace tyl::app
