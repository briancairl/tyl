/**
 * @copyright 2022-present Brian Cairl
 *
 * @file window.hpp
 */
#pragma once

// C++ Standard Library
#include <bitset>
#include <cstdint>
#include <utility>

namespace tyl
{

class Window
{
public:
  using KeyStates = std::bitset<16>;

  /**
   * @brief Keyboard key codes
   */
  struct KeyCodes
  {
    static constexpr std::size_t n0 = 0;
    static constexpr std::size_t n1 = 1;
    static constexpr std::size_t n2 = 2;
    static constexpr std::size_t n3 = 3;
    static constexpr std::size_t n4 = 4;
    static constexpr std::size_t n5 = 5;
    static constexpr std::size_t n6 = 6;
    static constexpr std::size_t n7 = 7;
    static constexpr std::size_t n8 = 8;
    static constexpr std::size_t n9 = 9;
    static constexpr std::size_t W = 10;
    static constexpr std::size_t A = 11;
    static constexpr std::size_t S = 12;
    static constexpr std::size_t D = 13;
  };

  /**
   * @brief Stores a window's size in pixels
   */
  struct Size
  {
    int height;
    int width;
  };

  /**
   * @brief Stores the position of the cursor on the screen
   *
   * Possibly holds sub-pixel position represented as fractions of whole pixels
   */
  struct Cursor
  {
    /// X-coordinate of cursor; oriented downward from top left corner of window
    double x;

    /// Y-coordinate of cursor; oriented rightward from top left corner of window
    double y;
  };

  /**
   * @brief Stores all relevant current window state
   */
  struct State
  {
    /// Current window size, in pixels
    Size size;

    /// Current window aspect ration
    double aspect_ratio;

    /// Current mouse cursor position, in pixels
    Cursor cursor_position;

    /// Current mouse cursor position in normalized screen coordinates adjusted by aspect ratio
    Cursor cursor_position_normalized;

    /// Most recent key-pressed events represented as a bit-field
    KeyStates key_pressed_flags;

    /// Most recent key-release events represented as a bit-field
    KeyStates key_released_flags;

    /// Most recent key-held events represented as a bit-field
    KeyStates key_held_flags;
  };

  struct Options
  {
    struct Flags
    {
      std::uint8_t enable_sticky_keys : 1;
      std::uint8_t enable_vsync : 1;
    };

    const char* title;
    Size size = {.height = 500, .width = 500};
    Flags flags = {.enable_sticky_keys = false, .enable_vsync = true};
  };

  explicit Window(Options&& options);

  ~Window();

  /**
   * @brief Runs window update loop until window is closed
   */
  template <typename LoopUpdateFnT> void loop(LoopUpdateFnT loop_update)
  {
    while (window_state_update())
    {
      loop_update(window_state_);
    }
  }

  /**
   * @brief Runs window update once, unless window is closed
   */
  template <typename LoopUpdateFnT> bool once(LoopUpdateFnT loop_update)
  {
    if (window_state_update())
    {
      loop_update(window_state_);
      return true;
    }
    return false;
  }

  /**
   * @copydoc loop
   */
  template <typename LoopUpdateFnT> void operator()(LoopUpdateFnT&& loop_update)
  {
    loop(std::forward<LoopUpdateFnT>(loop_update));
  }

private:
  bool window_state_update();

  /// Opaque pointer to active window (implementation defined)
  void* window_handle_ = nullptr;

  /// Current window state
  State window_state_ = {Size{0, 0}, 1.0, Cursor{0, 0}};
};

}  // namespace tyl
