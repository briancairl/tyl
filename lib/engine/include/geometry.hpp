/**
 * @copyright 2021-present Brian Cairl
 *
 * @file geometry.hpp
 */
#pragma once

// Tyl
#include <tyl/common/matrix.hpp>
#include <tyl/common/strong_alias.hpp>

namespace tyl::engine
{

TYL_DEFINE_STRONG_ALIAS(Transform, Mat3f);

struct UnitConversion
{
  inline float to_meters(const int pixels) { return pixels * pixels_per_meter; }
  inline int to_pixels(const float meters) { return meters / pixels_per_meter; }

  /// Number of screen pixels per meter at the default zoom level
  float pixels_per_meter;
};

}  // namespace tyl::engine
