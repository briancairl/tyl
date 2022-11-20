/**
 * @copyright 2022-present Brian Cairl
 *
 * @file window.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <utility>

// Tyl
#include <tyl/utility/bitfield.hpp>

namespace tyl
{

class Window
{
public:
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
    struct KeyFlags : bitfield<KeyFlags, std::uint64_t, 1>
    {
      std::uint64_t w : 1;
      std::uint64_t a : 1;
      std::uint64_t s : 1;
      std::uint64_t d : 1;
      std::uint64_t q : 1;
      std::uint64_t e : 1;
      std::uint64_t z : 1;
      std::uint64_t c : 1;
      std::uint64_t num0 : 1;
      std::uint64_t num1 : 1;
      std::uint64_t num2 : 1;
      std::uint64_t num3 : 1;
      std::uint64_t num4 : 1;
      std::uint64_t num5 : 1;
      std::uint64_t num6 : 1;
      std::uint64_t num7 : 1;
      std::uint64_t num8 : 1;
      std::uint64_t num9 : 1;
    };

    /// Current window size, in pixels
    Size size;

    /// Current window aspect ration
    double aspect_ratio;

    /// Current mouse cursor position, in pixels
    Cursor cursor_position;

    /// Current mouse cursor position in normalized screen coordinates adjusted by aspect ratio
    Cursor cursor_position_normalized;

    /// Most recent key-pressed events represented as a bit-field
    KeyFlags key_pressed_flags;

    /// Most recent key-release events represented as a bit-field
    KeyFlags key_released_flags;

    /// Most recent key-held events represented as a bit-field
    KeyFlags key_held_flags;
  };

  struct Options
  {
    struct Flags : bitfield<Flags, std::uint8_t, 1>
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
