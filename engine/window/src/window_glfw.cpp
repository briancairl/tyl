// C++ Standard Library
#include <algorithm>
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
#include <tyl/assert.hpp>
#include <tyl/engine/window.hpp>

namespace tyl::engine
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
      previous.reset();
    }
    else
    {
      previous.code = KeyState::RELEASED;
    }
    break;
  }
  default: {
    previous.reset();
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

void glfw_window_scroll_callback(GLFWwindow* const window, double xoffset, double yoffset)
{
  auto* const app_state_ptr = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));
  TYL_ASSERT_NON_NULL(app_state_ptr);
  app_state_ptr->cursor_scroll = {static_cast<float>(yoffset), static_cast<float>(xoffset)};
  app_state_ptr->cursor_scroll_stamp = app_state_ptr->now;

  if (
    app_state_ptr->previous_callbacks.scroll and
    app_state_ptr->previous_callbacks.scroll != glfw_window_scroll_callback)
  {
    reinterpret_cast<GLFWscrollfun>(app_state_ptr->previous_callbacks.scroll)(window, xoffset, yoffset);
  }
}

void glfw_window_drop_callback(GLFWwindow* const window, int path_count, const char* paths[])
{
  auto* const app_state_ptr = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));
  TYL_ASSERT_NON_NULL(app_state_ptr);

  // Set drop payload
  app_state_ptr->drop_payloads.reserve(path_count);
  std::transform(
    paths,
    paths + path_count,
    std::back_inserter(app_state_ptr->drop_payloads),
    [](const char* path) -> std::filesystem::path { return std::filesystem::path{path}; });

  // Set location of drop on screen
  {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    app_state_ptr->drop_cursor_position << xpos, ypos;
  }

  if (app_state_ptr->previous_callbacks.drop and app_state_ptr->previous_callbacks.drop != glfw_window_drop_callback)
  {
    reinterpret_cast<GLFWdropfun>(app_state_ptr->previous_callbacks.drop)(window, path_count, paths);
  }
}

void glfw_window_setup_callbacks(GLFWwindow* const window, Window::State* const app_state_ptr)
{
  glfwSetWindowUserPointer(window, reinterpret_cast<void*>(app_state_ptr));
  app_state_ptr->previous_callbacks.scroll =
    reinterpret_cast<void*>(glfwSetScrollCallback(window, glfw_window_scroll_callback));
  app_state_ptr->previous_callbacks.drop =
    reinterpret_cast<void*>(glfwSetDropCallback(window, glfw_window_drop_callback));
}

void glfw_window_restore_callbacks(GLFWwindow* const window)
{
  auto* const app_state_ptr = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));
  glfwSetScrollCallback(window, reinterpret_cast<GLFWscrollfun>(app_state_ptr->previous_callbacks.scroll));
  glfwSetDropCallback(window, reinterpret_cast<GLFWdropfun>(app_state_ptr->previous_callbacks.drop));
  app_state_ptr->previous_callbacks.scroll = nullptr;
  app_state_ptr->previous_callbacks.drop = nullptr;
  glfwSetWindowUserPointer(window, nullptr);
}

constexpr int kGLFWKeyCodes[KeyInfo::kKeyCount] = {
  GLFW_KEY_1,
  GLFW_KEY_2,
  GLFW_KEY_3,
  GLFW_KEY_4,
  GLFW_KEY_5,
  GLFW_KEY_6,
  GLFW_KEY_7,
  GLFW_KEY_8,
  GLFW_KEY_9,
  GLFW_KEY_0,
  GLFW_KEY_Q,
  GLFW_KEY_W,
  GLFW_KEY_E,
  GLFW_KEY_A,
  GLFW_KEY_S,
  GLFW_KEY_D,
  GLFW_KEY_Z,
  GLFW_KEY_X,
  GLFW_KEY_C,
  GLFW_KEY_SPACE,
  GLFW_KEY_LEFT_SHIFT,
  GLFW_KEY_RIGHT_SHIFT,
  GLFW_KEY_LEFT_CONTROL,
  GLFW_KEY_RIGHT_CONTROL,
  GLFW_KEY_LEFT_ALT,
  GLFW_KEY_RIGHT_ALT,
};

GLFWwindow* asGLFWwindow(void* p) { return reinterpret_cast<GLFWwindow*>(p); }
ImGuiContext* asImGuiContext(void* p) { return reinterpret_cast<ImGuiContext*>(p); }

}  // namespace

std::ostream& operator<<(std::ostream& os, const WindowCreationError error_code)
{
  switch (error_code)
  {
  case WindowCreationError::kApplicationBackendInitializationFailure: {
    return os << "tyl::engine::WindowCreationError::kApplicationBackendInitializationFailure";
  }
  case WindowCreationError::kGraphicsBackendInitializationFailure: {
    return os << "tyl::engine::WindowCreationError::kGraphicsBackendInitializationFailure";
  }
  case WindowCreationError::kEngineInitializationFailure: {
    return os << "tyl::engine::WindowCreationError::kEngineInitializationFailure";
  }
  case WindowCreationError::kWindowCreationFailure: {
    return os << "tyl::engine::WindowCreationError::kWindowCreationFailure";
  }
  }
  return os << "tyl::engine::WindowCreationError::*";
}

expected<Window, WindowCreationError> Window::create(const Options& options)
{
  if (!glfw_try_init())
  {
    return unexpected<WindowCreationError>{WindowCreationError::kApplicationBackendInitializationFailure};
  }

  // Create window with graphics context
  GLFWwindow* window =
    glfwCreateWindow(options.initial_window_width, options.initial_window_height, options.window_title, NULL, NULL);

  if (window == nullptr)
  {
    return unexpected<WindowCreationError>{WindowCreationError::kWindowCreationFailure};
  }


  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    return unexpected<WindowCreationError>{WindowCreationError::kGraphicsBackendInitializationFailure};
  }

  if (options.enable_vsync)
  {
    glfwSwapInterval(1);  // Enable vsync
  }

  // Setup Dear ImGui context
  if (imgui_contexts_active == 0)
  {
    IMGUI_CHECKVERSION();
  }

  WindowState window_state;

  if (auto* imgui_context = ImGui::CreateContext(); imgui_context == nullptr)
  {
    return unexpected<WindowCreationError>{WindowCreationError::kEngineInitializationFailure};
  }
  else
  {
    window_state.gui_context = imgui_context;
    ImGui::SetCurrentContext(imgui_context);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }

  // Setup style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ++imgui_contexts_active;

  return Window{reinterpret_cast<void*>(window), std::move(window_state), options.behavior};
}

Window::Window(void* const window_handle, State&& window_state, const WindowBehaviorOptions& behavior_options) :
    window_state_{std::move(window_state)}, window_handle_{window_handle}, behavior_options_{behavior_options}
{
  glfw_window_setup_callbacks(asGLFWwindow(window_handle_), &window_state_);
}

Window::Window(Window&& other) :
    window_state_{std::move(other.window_state_)},
    window_handle_{other.window_handle_},
    behavior_options_{other.behavior_options_}
{
  other.window_handle_ = nullptr;
  glfw_window_restore_callbacks(asGLFWwindow(window_handle_));
  glfw_window_setup_callbacks(asGLFWwindow(window_handle_), &window_state_);
}

Window::~Window()
{
  if (window_handle_ == nullptr)
  {
    return;
  }
  else
  {
    {
      auto* const glfw_window_handle = asGLFWwindow(window_handle_);
      glfwDestroyWindow(glfw_window_handle);
    }

    if (imgui_contexts_active == 1)
    {
      ImGui_ImplOpenGL3_Shutdown();
      --imgui_contexts_active;
    }

    ImGui::DestroyContext(asImGuiContext(window_state_.gui_context));
  }
}

WindowStatus Window::Begin()
{
  auto* const glfw_window_handle = asGLFWwindow(window_handle_);

  glfwMakeContextCurrent(glfw_window_handle);

  ImGui::SetCurrentContext(asImGuiContext(window_state_.gui_context));
  if (glfwWindowShouldClose(glfw_window_handle))
  {
    return WindowStatus::kClosing;
  }
  else
  {
    glfwPollEvents();

    window_state_.now = Clock::now();

    if (ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))
    {
      // Reset horizontal/vertical scroll state
      window_state_.cursor_scroll = {0, 0};

      // Reset all key states
      for (std::size_t i = 0; i < KeyInfo::kKeyCount; ++i)
      {
        window_state_.key_info.state[i].reset();
      }
    }
    else
    {
      // Scan for states of all keys of interest
      for (std::size_t i = 0; i < KeyInfo::kKeyCount; ++i)
      {
        glfw_get_key_state(window_state_.key_info.state[i], glfw_window_handle, kGLFWKeyCodes[i]);
      }

      // Get current cursor position on screen
      {
        double xpos, ypos;
        glfwGetCursorPos(glfw_window_handle, &xpos, &ypos);
        window_state_.cursor_position << xpos, ypos;
      }

      // Get current cursose position in graphics viewport space
      {
        window_state_.cursor_position_normalized =
          (1.0f - 2.f * window_state_.cursor_position.array() / window_state_.window_size.cast<float>().array());
        window_state_.cursor_position_normalized[0] = -window_state_.cursor_position_normalized[0];
      }
    }

    if (const auto dt = window_state_.now - window_state_.cursor_scroll_stamp;
        dt > behavior_options_.scroll_timeout_duration)
    {
      window_state_.cursor_scroll = {0, 0};
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Style Settings");
    ImGui::ShowStyleEditor();
    ImGui::End();

    return WindowStatus::kRunning;
  }
}

void Window::End()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  auto* glfw_window_handle = asGLFWwindow(window_handle_);
  int x_size, y_size;
  glfwGetFramebufferSize(glfw_window_handle, &x_size, &y_size);
  glViewport(0, 0, x_size, y_size);
  glfwSwapBuffers(glfw_window_handle);
  window_state_.window_size[0] = x_size;
  window_state_.window_size[1] = y_size;
}

}  // namespace tyl::engine
