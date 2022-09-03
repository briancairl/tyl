/**
 * @copyright 2022-present Brian Cairl
 *
 * @file render_target.hpp
 */
#pragma once

// Tyl
#include <tyl/math/rect.hpp>

namespace tyl::graphics
{

struct RenderTarget2D
{
  /// Viewport rect
  Rect2f view_rect;

  /// View projection transform
  Mat3f view_projection;
};

}  // namespace tyl::graphics

// Tyl
#include <tyl/serial.hpp>
#include <tyl/serial/math/rect.hpp>
#include <tyl/serial/math/vec.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>::tyl::graphics::tags::rendering_enabled</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, ::tyl::graphics::RenderTarget2D>
{
  void operator()(ArchiveT& ar, ::tyl::graphics::RenderTarget2D& render_target)
  {
    ar& named{"view_rect", render_target.view_rect};
    ar& named{"view_projection", render_target.view_projection};
  }
};

}  // namespace tyl::serialization
