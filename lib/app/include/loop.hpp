/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

// Tyl
#include <tyl/bitops.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/time.hpp>
#include <tyl/vec.hpp>

namespace tyl::app
{

/**
 * @brief Describes user input for a specific application Loop
 */
struct UserInput
{
  /// Current mouse pointer position (possible sub-pixel resolution)
  Vec2d cursor_position_full_resolution;

  /// Current mouse pointer position in the normalized device frame
  Vec2f cursor_position_normalized;

  /// Toggles when bitmask is pressed
  std::uint64_t previous_input_down_mask = 0;

  /// Toggles when bitmask is released
  std::uint64_t previous_input_up_mask = 0;

  /// Toggles when bitmask is pressed
  std::uint64_t input_down_mask = 0;

  /// Toggles when bitmask is up
  std::uint64_t input_up_mask = 0;

  /// Toggles when bitmask is pressed
  std::uint64_t input_pressed_mask = 0;

  /// Toggles when bitmask is up
  std::uint64_t input_released_mask = 0;

  static constexpr std::uint64_t MoveUp = tyl::bitops::make_mask<std::uint64_t, 0>();
  static constexpr std::uint64_t MoveDown = tyl::bitops::make_mask<std::uint64_t, 1>();
  static constexpr std::uint64_t MoveRight = tyl::bitops::make_mask<std::uint64_t, 2>();
  static constexpr std::uint64_t MoveLeft = tyl::bitops::make_mask<std::uint64_t, 3>();
  static constexpr std::uint64_t Sprint = tyl::bitops::make_mask<std::uint64_t, 4>();
  static constexpr std::uint64_t Jump = tyl::bitops::make_mask<std::uint64_t, 5>();
  static constexpr std::uint64_t LMB = tyl::bitops::make_mask<std::uint64_t, 61>();
  static constexpr std::uint64_t RMB = tyl::bitops::make_mask<std::uint64_t, 62>();
  static constexpr std::uint64_t MMB = tyl::bitops::make_mask<std::uint64_t, 63>();

  constexpr bool is_down(const std::uint64_t mask_query) const { return mask_query & input_down_mask; }
  constexpr bool is_up(const std::uint64_t mask_query) const { return mask_query & input_up_mask; }
  constexpr bool is_pressed(const std::uint64_t mask_query) const { return mask_query & input_pressed_mask; }
  constexpr bool released_up(const std::uint64_t mask_query) const { return mask_query & input_released_mask; }

  UserInput() = default;
};

/**
 * @brief Main application loop
 */
class Loop
{
public:
  Loop(const char* name, const Vec2i& window_size);

  ~Loop();

  int run(const std::function<bool(graphics::Target&, const UserInput&, const duration dt)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  UserInput user_input_;
  graphics::Target window_render_target_;
};

}  // namespace tyl::app
