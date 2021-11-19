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
#include <tyl/engine/app.hpp>
#include <tyl/logging/logging.hpp>
#include <tyl/ui/style.hpp>

namespace tyl::engine
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

}  // namespace anonymous


App::App(const char* name, const WindowSize& size) : window_name_{name}, window_ctx_{nullptr}, window_size_{size}
{
  logging::initialize();

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
  GLFWwindow* window = glfwCreateWindow(window_size_.width_px, window_size_.height_px, window_name_, NULL, NULL);

  if (window == NULL)
  {
    TYL_CRITICAL("[{}] glfwCreateWindow failed", window_name_);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

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

App::~App()
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

int App::run(const std::function<bool(const WindowSize&)>& loop_fn)
{
  GLFWwindow* window = reinterpret_cast<GLFWwindow*>(window_ctx_);

  ImVec4 background_color{0.1f, 0.1f, 0.1f, 1.0f};

  TYL_INFO("[{}] starting", window_name_);
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowStyleEditor(&imgui_style);
    // ImGui::ShowDemoWindow();

    if (!loop_fn(window_size_))
    {
      break;
    }

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &window_size_.width_px, &window_size_.height_px);
    glViewport(0, 0, window_size_.width_px, window_size_.height_px);
    glfwSwapBuffers(window);
  }
  TYL_INFO("[{}] closing", window_name_);
  return 0;
}

}  // namespace tyl::engine
