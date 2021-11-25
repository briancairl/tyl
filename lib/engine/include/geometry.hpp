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

struct Transform : Mat3f
{
  Transform(const float angle, const Vec2f offset)
  {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    // clang-format off
    (*this) << +c, -s,  offset.x(),
               +s, +c,  offset.y(),
                0,  0,  1;
    // clang-format on
  }

  inline static Transform translation(const Vec2f& offset)
  {
    Transform tf;
    // clang-format off
    tf << 1,  0,  offset.x(),
          0,  1,  offset.y(),
          0,  0,  1;
    // clang-format on
    return tf;
  }

  inline static Transform rotation(const float angle)
  {
    Transform tf;
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    // clang-format off
    tf << +c, -s,  0,
          +s, +c,  0,
           0,  0,  1;
    // clang-format on
    return tf;
  }

  using Mat3f::Mat3f;
  using Mat3f::operator=;
};

TYL_ENSURE_LAYOUT_COMPAT(Transform, Mat3f);

}  // namespace tyl::engine
