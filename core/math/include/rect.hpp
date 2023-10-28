/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// Tyl
#include <tyl/vec.hpp>

namespace tyl
{

template <typename T> class Rect : public Vec<T, 4>
{
  using BaseType = Vec<T, 4>;
  using Point = Vec<T, 2>;

public:
  Rect() = default;

  explicit Rect(const Vec<T, 4>& v) : BaseType{v} {}

  Rect(Point min_point, Point max_point)
  {
    std::memcpy(this->data() + 0, min_point.data(), sizeof(Point));
    std::memcpy(this->data() + 2, max_point.data(), sizeof(Point));
  }

  decltype(auto) min() { return this->template head<2>(); }
  decltype(auto) max() { return this->template tail<2>(); }
  decltype(auto) min() const { return this->template head<2>(); }
  decltype(auto) max() const { return this->template tail<2>(); }

  bool within(const Point& query) const
  {
    return (min().array() <= query.array()).all() and (max().array() > query.array()).all();
  }

  Point extents() const { return Point{max() - min()}; }
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
