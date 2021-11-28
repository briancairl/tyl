/**
 * @copyright 2021-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// C++ Standard Library
#include <initializer_list>

// Eigen
#include <Eigen/Dense>

namespace tyl
{

/**
 * @brief Describes an axis-aligned rectangular region
 */
template <typename T> class Rect
{
public:
  using corner_type = Eigen::Matrix<T, 2, 1>;

  Rect(const corner_type& min_corner, const corner_type& max_corner) : min_{min_corner}, max_{max_corner} {}

  constexpr const corner_type& min() const { return min_; }

  constexpr const corner_type& max() const { return max_; }

  constexpr const corner_type upper_left() const { return corner_type{min_.x(), max_.y()}; }

  constexpr const corner_type lower_right() const { return corner_type{max_.x(), min_.y()}; }

  constexpr const corner_type size() const { return (max_ - min_); }

  inline bool within(const corner_type& corner_point) const
  {
    return corner_point.x() >= min_.x() and corner_point.y() >= min_.y() and corner_point.x() < max_.x() and
      corner_point.y() < max_.y();
  }

  inline bool intersects(const Rect& other) const
  {
    return within(other.min()) or within(other.max()) or within(other.upper_left()) or within(other.lower_right());
  }

private:
  corner_type min_;
  corner_type max_;
};

using Rect2i = Rect<int>;

using Rect2f = Rect<float>;

}  // namespace tyl
