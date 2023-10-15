/**
 * @copyright 2023-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <cctype>
#include <filesystem>
#include <iosfwd>
#include <vector>

// Tyl
#include <tyl/clock.hpp>
#include <tyl/engine/keyboard.hpp>
#include <tyl/expected.hpp>
#include <tyl/vec.hpp>

namespace tyl::engine
{

struct WindowBehaviorOptions
{
  Clock::Duration scroll_timeout_duration = Clock::milliseconds(20);
};

struct WindowOptions
{
  int initial_window_height = 500;
  int initial_window_width = 500;
  const char* window_title = "app";
  bool enable_vsync = true;
  WindowBehaviorOptions behavior = {};
};

struct WindowCallbacks
{
  void* scroll = nullptr;
  void* drop = nullptr;
};

struct WindowState
{
  Clock::Time now = Clock::Time::min();
  Vec2i window_size = Vec2i::Zero();
  Vec2f cursor_position = Vec2f::Zero();
  Vec2f cursor_position_normalized = Vec2f::Zero();
  Vec2f cursor_scroll = {};
  Clock::Time cursor_scroll_stamp = Clock::Time::min();
  std::vector<std::filesystem::path> drop_payloads = {};
  Vec2f drop_cursor_position = Vec2f::Zero();
  KeyInfo key_info = {};
  void* gui_context = nullptr;
  WindowCallbacks previous_callbacks = {};
};

enum class WindowStatus
{
  kRunning,
  kClosing,
  kUpdateFailure
};

enum class WindowCreationError
{
  kApplicationBackendInitializationFailure,
  kGraphicsBackendInitializationFailure,
  kEngineInitializationFailure,
  kWindowCreationFailure
};

class Window
{
public:
  using Options = WindowOptions;
  using State = WindowState;

  Window(const Window& other) = delete;
  Window(Window&& other);

  ~Window();

  [[nodiscard]] static expected<Window, WindowCreationError> create(const Options& settings);

  template <typename UpdateStateCallbackT> WindowStatus update(UpdateStateCallbackT on_update)
  {
    const WindowStatus status = Begin();

    if (status != WindowStatus::kRunning)
    {
      return status;
    }

    if (!on_update(window_state_))
    {
      return WindowStatus::kUpdateFailure;
    }

    End();
    return status;
  };

private:
  WindowStatus Begin();
  void End();

  Window(void* const window_handle, State&& window_state, const WindowBehaviorOptions& window_behavior_options);

  State window_state_ = {};
  void* window_handle_ = nullptr;
  WindowBehaviorOptions behavior_options_ = {};
};

std::ostream& operator<<(std::ostream& os, const WindowCreationError error_code);

}  // namespace tyl::engine
