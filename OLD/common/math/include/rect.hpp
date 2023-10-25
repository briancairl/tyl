/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// Tyl
#include <tyl/math/vec.hpp>

namespace tyl
{

template <typename T> class Rect : public Vec<T, 4>
{
  using BaseType = Vec<T, 4>;
  using Vec2T = Vec<T, 2>;

public:
  explicit Rect(const Vec<T, 4>& v) : BaseType{v} {}

  Rect(Vec2T min_point, Vec2T max_point)
  {
    std::memcpy(this->data() + 0, min_point.data(), sizeof(Vec2T));
    std::memcpy(this->data() + 2, max_point.data(), sizeof(Vec2T));
  }

  decltype(auto) min() { return this->template head<2>(); }
  decltype(auto) max() { return this->template tail<2>(); }
  decltype(auto) min() const { return this->template head<2>(); }
  decltype(auto) max() const { return this->template tail<2>(); }

  bool within(const Vec2T& query) const
  {
    return (min().array() <= query.array()).all() and (max().array() > query.array()).all();
  }

  Vec2T extents() const { return Vec2T{max() - min()}; }
};

using Rect2i = Rect<int>;
using Rect2f = Rect<float>;

template <typename T> inline bool operator==(const Rect<T>& lhs, const Rect<T>& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(Rect<T>)) == 0;
}

template <typename T> inline bool operator!=(const Rect<T>& lhs, const Rect<T>& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(Rect<T>)) != 0;
}

}  // namespace tyl