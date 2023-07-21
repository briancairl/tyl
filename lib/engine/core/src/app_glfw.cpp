// C++ Standard Library
#include <atomic>
#include <cstdio>
#include <ostream>

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

// Tyl
#include <tyl/engine/core/app.hpp>

namespace tyl::engine::core
{
namespace
{

std::atomic_flag glfw_is_initialized = {false};
std::atomic<std::size_t> imgui_contexts_active = 0;

void glfw_error_callback(int error, const char* description) { std::fprintf(stderr, "%d : %s\n", error, description); }

bool glfw_try_init()
{
  if (!glfw_is_initialized.test_and_set())
  {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
      return false;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
  }
  return true;
}

void glfw_get_key_state(KeyState& previous, GLFWwindow* const window, const int keycode)
{
  switch (glfwGetKey(window, keycode))
  {
  case GLFW_PRESS: {
    if (previous.is_held())
    {
      break;
    }
    else if (previous.is_pressed())
    {
      previous.code = KeyState::HELD;
    }
    else
    {
      previous.code = KeyState::PRESSED;
    }
    break;
  }
  case GLFW_RELEASE: {
    if (previous.is_none())
    {
      break;
    }
    else if (previous.is_released())
    {
      previous.code = KeyState::NONE;
    }
    else
    {
      previous.code = KeyState::RELEASED;
    }
    break;
  }
  default: {
    previous.code = KeyState::NONE;
    break;
  }
  }
}

// Decide GL+GLSL versions
#if __APPLE__
// GL 3.2 + GLSL 150
constexpr const char* glsl_version = "#version 150";
#else
// GL 3.0 + GLSL 130
constexpr const char* glsl_version = "#version 130";  // 3.0+ only
#endif

}  // namespace

std::ostream& operator<<(std::ostream& os, const App::ErrorCode error_code)
{
  switch (error_code)
  {
  case App::ErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::ErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE";
  }
  case App::ErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::ErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE";
  }
  case App::ErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::ErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE";
  }
  case App::ErrorCode::WINDOW_CREATION_FAILURE: {
    return os << "tyl::engine::core::App::ErrorCode::WINDOW_CREATION_FAILURE";
  }
  }
  return os << "tyl::engine::core::App::ErrorCode::*";
}

App::App(App&& other) : window_state_{other.window_state_}, window_handle_{other.window_handle_}
{
  other.window_handle_ = nullptr;
}

expected<App, App::ErrorCode> App::create(const Options& options)
{
  if (!glfw_try_init())
  {
    return unexpected{ErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE};
  }

  // Create window with graphics context
  GLFWwindow* window =
    glfwCreateWindow(options.initial_window_height, options.initial_window_width, options.window_title, NULL, NULL);

  if (window == nullptr)
  {
    return unexpected{ErrorCode::WINDOW_CREATION_FAILURE};
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    return unexpected{ErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE};
  }

  if (options.enable_vsync)
  {
    glfwSwapInterval(1);  // Enable vsync
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGuiContext* const imgui_context = ImGui::CreateContext();
  if (imgui_context == nullptr)
  {
    return unexpected{ErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE};
  }
  else
  {
    ImGui::SetCurrentContext(imgui_context);
  }

  // Setup style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ++imgui_contexts_active;

  return App{reinterpret_cast<void*>(window), imgui_context};
}

App::App(void* const window_handle, ImGuiContext* const imgui_context) :
    window_state_{
      .window_size = {},
      .cursor_position = {},
      .cursor_position_normalized = {},
      .key_info = {},
      .imgui_context = imgui_context},
    window_handle_{window_handle}
{}

App::~App()
{
  if (window_handle_ == nullptr)
  {
    return;
  }
  else
  {
    if (imgui_contexts_active == 1)
    {
      ImGui_ImplOpenGL3_Shutdown();
      --imgui_contexts_active;
    }
    ImGui::DestroyContext(window_state_.imgui_context);
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(window_handle_));
  }
}

bool App::update_start()
{
  auto* glfw_window_handle = reinterpret_cast<GLFWwindow*>(window_handle_);
  glfwMakeContextCurrent(glfw_window_handle);

  ImGui::SetCurrentContext(window_state_.imgui_context);
  if (glfwWindowShouldClose(glfw_window_handle))
  {
    return false;
  }
  else
  {
    glfwPollEvents();

    glfw_get_key_state(window_state_.key_info.W, glfw_window_handle, GLFW_KEY_W);
    glfw_get_key_state(window_state_.key_info.S, glfw_window_handle, GLFW_KEY_S);
    glfw_get_key_state(window_state_.key_info.A, glfw_window_handle, GLFW_KEY_A);
    glfw_get_key_state(window_state_.key_info.D, glfw_window_handle, GLFW_KEY_D);

    {
      double xpos, ypos;
      glfwGetCursorPos(glfw_window_handle, &xpos, &ypos);
      window_state_.cursor_position[0] = xpos;
      window_state_.cursor_position[1] = ypos;
      window_state_.cursor_position_normalized =
        (window_state_.cursor_position.array() / window_state_.window_size.cast<float>().array());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    return true;
  }
}

void App::update_end()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  auto* glfw_window_handle = reinterpret_cast<GLFWwindow*>(window_handle_);
  int x_size, y_size;
  glfwGetFramebufferSize(glfw_window_handle, &x_size, &y_size);
  glViewport(0, 0, x_size, y_size);
  glfwSwapBuffers(glfw_window_handle);
  window_state_.window_size[0] = y_size;
  window_state_.window_size[1] = x_size;
}

}  // namespace tyl::engine::core
