/**
 * @copyright 2022-present Brian Cairl
 *
 * @file render_target.hpp
 */
#pragma once

// Tyl
#include <tyl/math/rect.hpp>

namespace tyl::graphics::systems
{

struct RenderTarget2D
{
  /// Viewport rect
  Rect2f view_rect;

  /// View projection transform
  Mat3f view_projection;
};

}  // namespace tyl::graphics::systems
