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
 * @brief Describes window / input state
 */
struct WindowProperties
{
  /// Current size of the render viewport
  ViewportSize viewport_size;

  /// Current mouse pointer position (possible sub-pixel resolution)
  Vec2d cursor_position_full_resolution;

  /// Current mouse pointer position in the normalized device frame
  Vec2f cursor_position_normalized;
};

// Abstract out world cursor to some : -interactor position(mouse in world frame) -
//                                     interactor roi(screen bounds in world frame)

}  // namespace tyl::engine
