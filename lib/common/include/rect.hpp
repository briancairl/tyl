/**
 * @copyright 2021-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

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

  /**
   * @brief Creates a Rect from corner locations
   */
  inline static Rect corners(const corner_type& min, const corner_type& max) { return Rect{min, max - min}; }

  /**
   * @brief Returns a rectangle, scaled from the center point of the original rectangle
   */
  inline static Rect scaled_from_center(const Rect<T>& rect, const T scaling)
  {
    const corner_type c{rect.center()};
    return Rect::corners(scaling * (rect.min() - c) + c, scaling * (rect.max() - c) + c);
  }

  Rect(const Rect&) = default;

  Rect(const corner_type& corner, const extents_type& extents) : corner_{corner}, extents_{extents} {}

  Rect() : Rect{corner_type::Zero(), extents_type::Zero()} {}

  /**
   * @brief Returns lower left corner of the rectangle
   */
  constexpr const corner_type& min() const { return corner_; }

  /**
   * @brief Returns upper right corner of the rectangle
   */
  constexpr const corner_type max() const { return corner_ + extents_; }

  /**
   * @brief Returns upper left corner of the rectangle
   */
  constexpr corner_type upper_left() const { return corner_type{corner_.x(), corner_.y() + extents_.y()}; }

  /**
   * @brief Returns lower right corner of the rectangle
   */
  constexpr corner_type lower_right() const { return corner_type{corner_.x() + extents_.x(), corner_.y()}; }

  /**
   * @brief Returns center point of the the rectangle
   */
  inline corner_type center() const
  {
    if constexpr (std::is_floating_point<T>())
    {
      static constexpr T MUL = 0.5;
      return corner_ + MUL * extents_;
    }
    else
    {
      static constexpr T DIV = 2;
      return corner_ + extents_ / DIV;
    }
  }

  /**
   * @brief Returns the positive extents of the rectangle along each axis
   */
  constexpr const extents_type& extents() const { return extents_; }

  /**
   * @brief Checks if a point is within the rectangular region
   */
  inline bool within(const corner_type& query) const { return within_impl(query, corner_, corner_ + extents_); }

  /**
   * @brief Checks if rectngle intersects (overlaps) with another rectangular region
   */
  inline bool intersects(const Rect& other) const
  {
    return intersects_impl(*this, other) or intersects_impl(other, *this);
  }

  /**
   * @brief Sets the lower left corner location of the rectangle to \c corner
   */
  inline void snap(const corner_type& corner) { corner_ = corner; }

  /**
   * @brief Offsets rectangle by \c offset
   */
  inline Rect& operator+=(const extents_type& offset)
  {
    corner_ += offset;
    return *this;
  }

  /**
   * @brief Offsets rectangle by negative \c offset
   */
  inline Rect& operator-=(const extents_type& offset)
  {
    corner_ -= offset;
    return *this;
  }

  /**
   * @brief Returns a rectangle offset by \c offset
   */
  inline Rect operator+(const extents_type& offset) const { return Rect{corner_ + offset, extents_}; }

  /**
   * @brief Returns a rectangle offset by negative \c offset
   */
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
