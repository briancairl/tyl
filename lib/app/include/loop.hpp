/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

// Tyl
#include <tyl/graphics/target.hpp>
#include <tyl/vec.hpp>

namespace tyl::app
{

/**
 * @brief Describes current application state
 */
struct WindowState
{
  /// Current mouse pointer position (possible sub-pixel resolution)
  Vec2d cursor_position_full_resolution;

  /// Current mouse pointer position in the normalized device frame
  Vec2f cursor_position_normalized;
};

/**
 * @brief Main application loop
 */
class Loop
{
public:
  Loop(const char* name, const Vec2i& window_size);

  ~Loop();

  int run(const std::function<bool(const graphics::Target&, const WindowState&)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  WindowState window_state_;
  graphics::Target window_render_target_;
};

}  // namespace tyl::app
