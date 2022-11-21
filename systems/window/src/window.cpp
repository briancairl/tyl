/**
 * @copyright 2022-present Brian Cairl
 *
 * @file window.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <exception>
#include <tuple>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/window/window.hpp>

namespace tyl
{
namespace
{
namespace glfw
{

void message_callback(int error, const char* description)
{
  std::fprintf(stderr, "[GLFW][%.6d] : %s\n", error, description);
}

int init()
{
  glfwSetErrorCallback(message_callback);
  if (!glfwInit())
  {
    std::terminate();
  }
  message_callback(0, "initialized");
  return 0;
}

template <std::size_t Bit, typename KeyCodeT>
void read_key_states(
  GLFWwindow* window,
  Window::KeyStates& pressed,
  Window::KeyStates& released,
  Window::KeyStates& held,
  const KeyCodeT key_code)
{
  const auto key_state = glfwGetKey(window, key_code);
  pressed.set(Bit, key_state == GLFW_PRESS);
  released.set(Bit, key_state == GLFW_RELEASE);
  held.set(Bit, key_state == GLFW_REPEAT);
}

template <typename KeyCodeTupleT, std::size_t... BitOffsets>
void read_many_key_states_impl(
  GLFWwindow* window,
  Window::KeyStates& pressed,
  Window::KeyStates& released,
  Window::KeyStates& held,
  KeyCodeTupleT&& key_codes,
  std::integer_sequence<std::size_t, BitOffsets...> bit_offsets)
{
  [[maybe_unused]] const auto _ =
    ((read_key_states<BitOffsets>(
        window, pressed, released, held, std::get<BitOffsets>(std::forward<KeyCodeTupleT>(key_codes))),
      0) +
     ...);
}

template <typename... KeyCodeTs>
void read_many_key_states(
  GLFWwindow* window,
  Window::KeyStates& pressed,
  Window::KeyStates& released,
  Window::KeyStates& held,
  KeyCodeTs... key_codes)
{
  read_many_key_states_impl(
    window,
    pressed,
    released,
    held,
    std::forward_as_tuple(key_codes...),
    std::make_integer_sequence<std::size_t, sizeof...(KeyCodeTs)>{});
}

}  // namespace glfw
}  // namespace

Window::Window(Options&& options) : window_state_{.size = options.size, .cursor_position = {0.0, 0.0}}
{
  // Initialize GLFW once
  [[maybe_unused]] static int _ = glfw::init();

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(options.size.width, options.size.height, options.title, NULL, NULL);

  if (window == NULL)
  {
    std::terminate();
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(options.flags.enable_vsync ? 1 : 0);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, options.flags.enable_sticky_keys ? GLFW_TRUE : GLFW_FALSE);

  window_handle_ = reinterpret_cast<void*>(window);
}

Window::~Window()
{
  auto* const window = reinterpret_cast<GLFWwindow*>(window_handle_);
  glfwDestroyWindow(window);
}

bool Window::window_state_update()
{
  auto* const window = reinterpret_cast<GLFWwindow*>(window_handle_);

  // Handle close out
  if (glfwWindowShouldClose(window))
  {
    return false;
  }

  // Get viewport size
  glfwGetFramebufferSize(window, &window_state_.size.width, &window_state_.size.height);

  // Set active GL frame buffer size
  glViewport(0, 0, window_state_.size.width, window_state_.size.height);

  // Swap buffers
  glfwSwapBuffers(window);

  // Poll window events
  glfwPollEvents();

  // Get cursor coordinates in pixel space
  glfwGetCursorPos(window, &window_state_.cursor_position.y, &window_state_.cursor_position.x);

  // Remaps mouse coordinates to [-1, 1]
  {
    const double h_w = 0.5 * window_state_.size.width;
    const double h_h = 0.5 * window_state_.size.height;
    window_state_.aspect_ratio = h_w / h_h;
    window_state_.cursor_position_normalized.x = (window_state_.cursor_position.x - h_h) / h_h;
    window_state_.cursor_position_normalized.y =
      (window_state_.cursor_position.y - h_w) / h_w * window_state_.aspect_ratio;
  }

  // Poll for key states
  glfw::read_many_key_states(
    window,
    window_state_.key_pressed_flags,
    window_state_.key_pressed_flags,
    window_state_.key_released_flags,
    GLFW_KEY_0,
    GLFW_KEY_1,
    GLFW_KEY_2,
    GLFW_KEY_3,
    GLFW_KEY_4,
    GLFW_KEY_5,
    GLFW_KEY_6,
    GLFW_KEY_7,
    GLFW_KEY_8,
    GLFW_KEY_9,
    GLFW_KEY_W,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D);

  // Clear frame buffer
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  return true;
};

}  // namespace tyl