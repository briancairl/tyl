/**
 * @copyright 2021-present Brian Cairl
 *
 * @file viewport.hpp
 */
#pragma once

// Tyl
#include <tyl/common/matrix.hpp>
#include <tyl/common/strong_alias.hpp>

namespace tyl::engine
{

/**
 * @brief Describes the size of the engine target region, in pixels
 */
TYL_DEFINE_STRONG_ALIAS(ViewportSize, Vec2i);

/**
 * @brief Describes pixel position of the cursor within the window with, possibly, full sub-pixel resolution
 */
TYL_DEFINE_STRONG_ALIAS(CursorPositionFullResolution, Vec2d);

/**
 * @brief Describes pixel position of the cursor within the window
 */
TYL_DEFINE_STRONG_ALIAS(CursorPosition, Vec2i);

/**
 * @brief Describes normalized position of the cursor within the window
 */
TYL_DEFINE_STRONG_ALIAS(CursorPositionNormalized, Vec2f);

/**
 * @brief Describes window / input state
 */
struct WindowProperties
{
  /// Current size of the render viewport
  ViewportSize viewport_size;

  /// Current mouse pointer position (possible sub-pixel resolution)
  CursorPositionFullResolution cursor_position_full_resolution;

  inline CursorPosition get_cursor_position() const
  {
    return CursorPosition(cursor_position_full_resolution.x(), cursor_position_full_resolution.y());
  }

  inline CursorPositionNormalized get_cursor_position_normalized() const
  {
    const float xn = cursor_position_full_resolution.x() / viewport_size.x();
    const float yn = cursor_position_full_resolution.y() / viewport_size.y();
    return CursorPositionNormalized{2.f * xn - 1.0f, 1.0f - 2.f * yn};
  }
};

}  // namespace tyl::engine
