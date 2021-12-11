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
  using extents_type = Eigen::Matrix<T, 2, 1>;

  inline static Rect corners(const corner_type& min, const corner_type& max) { return Rect{min, max - min}; }

  Rect(const Rect&) = default;

  Rect(const corner_type& corner, const extents_type& extents) : corner_{corner}, extents_{extents} {}

  Rect() : Rect{corner_type::Zero(), extents_type::Zero()} {}

  constexpr const corner_type& min() const { return corner_; }

  constexpr const corner_type max() const { return corner_ + extents_; }

  constexpr const corner_type upper_left() const { return corner_type{corner_.x(), corner_.y() + extents_.y()}; }

  constexpr const corner_type lower_right() const { return corner_type{corner_.x() + extents_.x(), corner_.y()}; }

  constexpr const extents_type& extents() const { return extents_; }

  inline bool within(const corner_type& query) const { return within_impl(query, corner_, corner_ + extents_); }

  inline bool intersects(const Rect& other) const
  {
    return intersects_impl(*this, other) or intersects_impl(other, *this);
  }

  inline void snap(const corner_type& corner) { corner_ = corner; }

  inline Rect& operator+=(const extents_type& offset)
  {
    corner_ += offset;
    return *this;
  }

  inline Rect& operator-=(const extents_type& offset)
  {
    corner_ -= offset;
    return *this;
  }

  inline Rect operator+(const extents_type& offset) const { return Rect{corner_ + offset, extents_}; }

  inline Rect operator-(const extents_type& offset) const { return Rect{corner_ - offset, extents_}; }

private:
  inline static bool within_impl(const corner_type& query, const corner_type& min, const corner_type& max)
  {
    return (query.array() >= min.array()).all() and (query.array() < max.array()).all();
  }

  inline static bool intersects_impl(const Rect& lhs, const Rect& rhs)
  {
    // clang-format off
    const corner_type lhs_max{lhs.corner_ + lhs.extents_};
    return within_impl(rhs.min(), lhs.corner_, lhs_max) or
           within_impl(rhs.max(), lhs.corner_, lhs_max) or
           within_impl(rhs.upper_left(), lhs.corner_, lhs_max) or
           within_impl(rhs.lower_right(), lhs.corner_, lhs_max);
    // clang-format on
  }

  corner_type corner_;
  extents_type extents_;
};


using Rect2i = Rect<int>;

using Rect2f = Rect<float>;

}  // namespace tyl
