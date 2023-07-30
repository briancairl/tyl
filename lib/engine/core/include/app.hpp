/**
 * @copyright 2023-present Brian Cairl
 *
 * @file resource.hpp
 */
#pragma once

// C++ Standard Library
#include <cctype>
#include <iosfwd>

// Entt
#include <entt/entt.hpp>

// Tyl
#include <tyl/engine/core/clock.hpp>
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

  constexpr void reset() { code = NONE; }
  constexpr bool is_none() const { return code == NONE; }
  constexpr bool is_released() const { return code == RELEASED; }
  constexpr bool is_pressed() const { return code == PRESSED; }
  constexpr bool is_held() const { return code == HELD; }
  constexpr bool is_down() const { return is_pressed() or is_held(); }
  constexpr bool is_up() const { return is_released() or is_none(); }

  KeyState& operator=(const KeyState& other) = default;

  KeyState& operator=(const KeyState::Code code)
  {
    this->code = code;
    return *this;
  };
};

struct KeyInfo
{
  enum KeyCode
  {
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    NUM6,
    NUM7,
    NUM8,
    NUM9,
    NUM0,
    Q,
    W,
    E,
    A,
    S,
    D,
    Z,
    X,
    C,
    SPACE,
    L_SHIFT,
    R_SHIFT,
    L_CTRL,
    R_CTRL,
    L_ALT,
    R_ALT,
    _KEY_COUNT,
  };

  static constexpr auto kKeyCount = static_cast<std::size_t>(_KEY_COUNT);

  constexpr KeyState& operator[](const KeyCode key) { return state[static_cast<std::size_t>(key)]; }
  constexpr const KeyState& operator[](const KeyCode key) const { return state[static_cast<std::size_t>(key)]; }

  constexpr auto* begin() { return state; }
  constexpr auto* end() { return state + kKeyCount; }

  constexpr const auto* begin() const { return state; }
  constexpr const auto* end() const { return state + kKeyCount; }

  KeyState state[kKeyCount];
};

struct AppOptions
{
  int initial_window_height = 500;
  int initial_window_width = 500;
  const char* window_title = "app";
  bool enable_vsync = true;
};

struct AppState
{
  Clock::Time now = Clock::Time::min();
  Vec2i window_size = Vec2i::Zero();
  Vec2f cursor_position = Vec2f::Zero();
  Vec2f cursor_position_normalized = Vec2f::Zero();
  Stamped<Vec2f> cursor_scroll = {};
  KeyInfo key_info = {};
  entt::registry* registry = nullptr;
  ImGuiContext* imgui_context = nullptr;
};

class App
{
public:
  using Options = AppOptions;
  using State = AppState;

  enum class OnCreateErrorCode
  {
    APPLICATION_BACKEND_INITIALIZATION_FAILURE,
    GRAPHICS_BACKEND_INITIALIZATION_FAILURE,
    ENGINE_GUI_INITIALIZATION_FAILURE,
    WINDOW_CREATION_FAILURE
  };

  App(const App& other) = delete;
  App(App&& other);

  ~App();

  [[nodiscard]] static expected<App, OnCreateErrorCode> create(const Options& settings);

  template <typename UpdateStateCallbackT> bool update(entt::registry& registry, UpdateStateCallbackT update_callback)
  {
    if (App::update_start(registry))
    {
      update_callback(*window_state_);
    }
    else
    {
      return false;
    }
    App::update_end(registry);
    return true;
  };

private:
  bool update_start(entt::registry& registry);
  void update_end(entt::registry& registry);

  App(void* const window_handle, std::unique_ptr<State>&& window_state);

  std::unique_ptr<State> window_state_ = {};
  void* window_handle_ = nullptr;
};

std::ostream& operator<<(std::ostream& os, const App::OnCreateErrorCode error_code);

}  // namespace tyl::engine::core
