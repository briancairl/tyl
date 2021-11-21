/**
 * @copyright 2021-present Brian Cairl
 *
 * @file viewport.hpp
 */
#pragma once

namespace tyl::render
{

/**
 * @brief Describes the size of the render target region, in pixels
 */
struct ViewportSize
{
  /// Height of the viewport, in pixels
  int height_px;

  /// Width of the viewport, in pixels
  int width_px;
};

}  // namespace tyl::render
