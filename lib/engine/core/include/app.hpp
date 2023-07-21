/**
 * @copyright 2023-present Brian Cairl
 *
 * @file resource.hpp
 */
#pragma once

// C++ Standard Library
#include <cctype>
#include <iosfwd>

// ImGui
#include <imgui.h>

// Tyl
#include <tyl/math/vec.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::engine::core
{

struct KeyState
{
  enum Code : std::uint8_t
  {
    PRESSED,
    RELEASED,
    HELD,
    NONE
  };

  Code code = NONE;

  constexpr bool is_none() const { return code == NONE; }
  constexpr bool is_released() const { return code == RELEASED; }
  constexpr bool is_pressed() const { return code == PRESSED; }
  constexpr bool is_held() const { return code == HELD; }
  constexpr bool is_down() const { return is_pressed() or is_held(); }
  constexpr bool is_up() const { return is_released() or is_none(); }
};

struct KeyInfo
{
  KeyState Q;
  KeyState W;
  KeyState E;
  KeyState A;
  KeyState S;
  KeyState D;
  KeyState Z;
  KeyState X;
  KeyState C;
  KeyState ARROW_UP;
  KeyState ARROW_DOWN;
  KeyState ARROW_L;
  KeyState ARROW_R;
};

struct AppOptions
{
  int initial_window_height = 500;
  int initial_window_width = 500;
  const char* window_title = "app";
  bool enable_vsync = true;
};

class App
{
public:
  using Options = AppOptions;

  struct State
  {
    Vec2i window_size;
    Vec2f cursor_position;
    Vec2f cursor_position_normalized;
    KeyInfo key_info;
    ImGuiContext* imgui_context;
  };

  enum class ErrorCode
  {
    APPLICATION_BACKEND_INITIALIZATION_FAILURE,
    GRAPHICS_BACKEND_INITIALIZATION_FAILURE,
    ENGINE_GUI_INITIALIZATION_FAILURE,
    WINDOW_CREATION_FAILURE
  };

  App(const App& other) = delete;
  App(App&& other);

  ~App();

  [[nodiscard]] static expected<App, ErrorCode> create(const Options& settings);

  template <typename UpdateStateCallbackT> bool update(UpdateStateCallbackT update_callback)
  {
    if (App::update_start())
    {
      update_callback(window_state_);
    }
    else
    {
      return false;
    }
    App::update_end();
    return true;
  };

private:
  bool update_start();
  void update_end();

  App(void* const window_handle, ImGuiContext* const imgui_context);

  State window_state_ = {};
  void* window_handle_ = nullptr;
};

std::ostream& operator<<(std::ostream& os, const App::ErrorCode error_code);

}  // namespace tyl::engine::core
