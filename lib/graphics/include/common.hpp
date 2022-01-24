/**
 * @copyright 2022-present Brian Cairl
 *
 * @file common.hpp
 */
#pragma once

namespace tyl::graphics
{

/**
 * @brief Tag used to signal rendering
 */
struct RenderingEnabled
{};

/**
 * @brief Z-sorting height for render ordering
 */
struct RenderOrder2D
{
  /// Fixed z-ordering component
  std::size_t z;

  /// Implicit y-ordering component
  float y;
};

constexpr bool operator<(const RenderOrder2D& lhs, const RenderOrder2D& rhs) { return lhs.z < rhs.z and lhs.y < rhs.y; }
constexpr bool operator>(const RenderOrder2D& lhs, const RenderOrder2D& rhs) { return lhs.z > rhs.z and lhs.y > rhs.y; }
constexpr bool operator==(const RenderOrder2D& lhs, const RenderOrder2D& rhs)
{
  return lhs.z == rhs.z and lhs.y == rhs.y;
}
constexpr bool operator<=(const RenderOrder2D& lhs, const RenderOrder2D& rhs)
{
  return operator==(lhs, rhs) or operator<(lhs, rhs);
}
constexpr bool operator>=(const RenderOrder2D& lhs, const RenderOrder2D& rhs)
{
  return operator==(lhs, rhs) or operator>(lhs, rhs);
}
constexpr bool operator!=(const RenderOrder2D& lhs, const RenderOrder2D& rhs) { return !operator==(lhs, rhs); }

}  // namespace tyl::graphics
