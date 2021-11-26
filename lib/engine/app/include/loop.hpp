/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

// Tyl
#include <tyl/matrix.hpp>

namespace tyl::app
{

/**
 * @brief Describes current application state
 */
struct State
{
  /// Current size of the render viewport
  Vec2i viewport_size;

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

  int run(const std::function<bool(const State&)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  State window_state_;
};

}  // namespace tyl::app
