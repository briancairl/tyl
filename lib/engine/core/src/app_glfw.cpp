// C++ Standard Library
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <ostream>
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

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/app.hpp>
#include <tyl/engine/core/clock.hpp>
#include <tyl/engine/core/drag_and_drop.hpp>
#include <tyl/utility/entt.hpp>


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
  auto* const app_state_ptr = reinterpret_cast<AppState*>(glfwGetWindowUserPointer(window));
  TYL_ASSERT_NON_NULL(app_state_ptr);
  app_state_ptr->cursor_scroll.emplace(app_state_ptr->now, static_cast<float>(yoffset), static_cast<float>(xoffset));
}

void glfw_window_drag_and_drop_callback(GLFWwindow* const window, int path_count, const char* paths[])
{
  auto* const app_state_ptr = reinterpret_cast<AppState*>(glfwGetWindowUserPointer(window));
  TYL_ASSERT_NON_NULL(app_state_ptr);

  if (app_state_ptr->registry == nullptr)
  {
    return;
  }
  else if (auto* const drag_and_drop_data_ptr = get_or_emplace<DragAndDropData>(*app_state_ptr->registry);
           drag_and_drop_data_ptr == nullptr)
  {
    return;
  }
  else
  {
    drag_and_drop_data_ptr->paths.reserve(drag_and_drop_data_ptr->paths.size() + path_count);
    std::transform(
      paths,
      paths + path_count,
      std::back_inserter(drag_and_drop_data_ptr->paths),
      [](const char* path_cstr) -> std::filesystem::path { return std::filesystem::path{path_cstr}; });
  }
}

bool glfw_window_set_callbacks(GLFWwindow* const window, App::State* const app_state)
{
  glfwSetWindowUserPointer(window, reinterpret_cast<void*>(app_state));

  if (glfwSetScrollCallback(window, glfw_window_scroll_callback) != nullptr)
  {
    return false;
  }
  else if (glfwSetDropCallback(window, glfw_window_drag_and_drop_callback) != nullptr)
  {
    return false;
  }
  else
  {
    return true;
  }
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

}  // namespace

std::ostream& operator<<(std::ostream& os, const App::OnCreateErrorCode error_code)
{
  switch (error_code)
  {
  case App::OnCreateErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::OnCreateErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE";
  }
  case App::OnCreateErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::OnCreateErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE";
  }
  case App::OnCreateErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE: {
    return os << "tyl::engine::core::App::OnCreateErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE";
  }
  case App::OnCreateErrorCode::WINDOW_CREATION_FAILURE: {
    return os << "tyl::engine::core::App::OnCreateErrorCode::WINDOW_CREATION_FAILURE";
  }
  }
  return os << "tyl::engine::core::App::OnCreateErrorCode::*";
}

expected<App, App::OnCreateErrorCode> App::create(const Options& options)
{
  if (!glfw_try_init())
  {
    return unexpected{OnCreateErrorCode::APPLICATION_BACKEND_INITIALIZATION_FAILURE};
  }

  // Create window with graphics context
  GLFWwindow* window =
    glfwCreateWindow(options.initial_window_width, options.initial_window_height, options.window_title, NULL, NULL);

  if (window == nullptr)
  {
    return unexpected{OnCreateErrorCode::WINDOW_CREATION_FAILURE};
  }

  auto window_state_ptr = std::make_unique<AppState>();

  if (!glfw_window_set_callbacks(window, window_state_ptr.get()))
  {
    return unexpected{OnCreateErrorCode::WINDOW_CREATION_FAILURE};
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    return unexpected{OnCreateErrorCode::GRAPHICS_BACKEND_INITIALIZATION_FAILURE};
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

  window_state_ptr->imgui_context = ImGui::CreateContext();
  if (window_state_ptr->imgui_context == nullptr)
  {
    return unexpected{OnCreateErrorCode::ENGINE_GUI_INITIALIZATION_FAILURE};
  }
  else
  {
    ImGui::SetCurrentContext(window_state_ptr->imgui_context);
  }

  // Setup style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ++imgui_contexts_active;

  return App{reinterpret_cast<void*>(window), std::move(window_state_ptr)};
}

App::App(void* const window_handle, std::unique_ptr<State>&& window_state) :
    window_state_{std::move(window_state)}, window_handle_{window_handle}
{}

App::App(App&& other) : window_state_{std::move(other.window_state_)}, window_handle_{other.window_handle_}
{
  other.window_handle_ = nullptr;
}

App::~App()
{
  if (window_handle_ == nullptr)
  {
    return;
  }
  else
  {
    {
      auto* const glfw_window_handle = reinterpret_cast<GLFWwindow*>(window_handle_);
      glfwDestroyWindow(glfw_window_handle);
    }

    if (imgui_contexts_active == 1)
    {
      ImGui_ImplOpenGL3_Shutdown();
      --imgui_contexts_active;
    }

    ImGui::DestroyContext(window_state_->imgui_context);
  }
}

bool App::update_start(entt::registry& registry)
{
  auto* const glfw_window_handle = reinterpret_cast<GLFWwindow*>(window_handle_);

  glfwMakeContextCurrent(glfw_window_handle);

  ImGui::SetCurrentContext(window_state_->imgui_context);
  if (glfwWindowShouldClose(glfw_window_handle))
  {
    return false;
  }
  else
  {
    glfwPollEvents();

    window_state_->now = Clock::now();
    window_state_->registry = std::addressof(registry);

    // Scan for states of all keys of interest
    for (std::size_t i = 0; i < KeyInfo::kKeyCount; ++i)
    {
      glfw_get_key_state(window_state_->key_info.state[i], glfw_window_handle, kGLFWKeyCodes[i]);
    }

    // Get current cursor position on screen
    {
      double xpos, ypos;
      glfwGetCursorPos(glfw_window_handle, &xpos, &ypos);
      window_state_->cursor_position << xpos, ypos;
    }

    // Get current cursose position in graphics viewport space
    {
      window_state_->cursor_position_normalized =
        (1.0f - 2.f * window_state_->cursor_position.array() / window_state_->window_size.cast<float>().array());
      window_state_->cursor_position_normalized[0] = -window_state_->cursor_position_normalized[0];
    }

    // Reset horizontal/vertical scroll state
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
    {
      window_state_->cursor_scroll.reset();
    }
    else if (const auto dt = window_state_->now - window_state_->cursor_scroll.stamp(); dt > Clock::millis(20))
    {
      window_state_->cursor_scroll.reset();
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

void App::update_end(entt::registry& registry)
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  auto* glfw_window_handle = reinterpret_cast<GLFWwindow*>(window_handle_);
  int x_size, y_size;
  glfwGetFramebufferSize(glfw_window_handle, &x_size, &y_size);
  glViewport(0, 0, x_size, y_size);
  glfwSwapBuffers(glfw_window_handle);
  window_state_->window_size[0] = x_size;
  window_state_->window_size[1] = y_size;

  if (auto* const drag_and_drop_data_ptr = get<DragAndDropData>(registry); drag_and_drop_data_ptr != nullptr)
  {
    drag_and_drop_data_ptr->paths.clear();
  }
}

}  // namespace tyl::engine::core
