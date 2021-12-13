/**
 * @copyright 2021 Tyl
 * @author Brian Cairl
 */

// C++ Standard Library
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/rect.hpp>
#include <tyl/vec.hpp>

using namespace tyl;

TEST(RectWithin, PointWithin)
{
  const Rect rect{Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f}};
  ASSERT_TRUE(rect.within(Vec2f{0.f, 0.f}));
}

TEST(RectWithin, PointAtCorners)
{
  const Rect rect{Rect<float>::corners(Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f})};
  ASSERT_TRUE(rect.within(Vec2f{-1.f, 2.f}));
  ASSERT_FALSE(rect.within(Vec2f{3.f, 2.f}));
  ASSERT_FALSE(rect.within(Vec2f{-1.f, 4.f}));
  ASSERT_FALSE(rect.within(Vec2f{3.f, 4.f}));
}

TEST(RectWithin, PointOutside)
{
  const Rect rect{Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f}};
  ASSERT_FALSE(rect.within(Vec2f{10.f, 10.f}));
}

TEST(RectIntersects, FullOverlap)
{
  const Rect large{Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f}};
  const Rect small{Vec2f{0.f, 0.f}, Vec2f{1.f, 1.f}};

  ASSERT_TRUE(large.intersects(small));
  ASSERT_TRUE(small.intersects(large));
}

TEST(RectIntersects, CornerOverlap)
{
  const Rect lower{Rect<float>::corners(Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f})};
  const Rect upper{Rect<float>::corners(Vec2f{3.f, 4.f}, Vec2f{5.f, 5.f})};

  ASSERT_TRUE(lower.intersects(upper));
  ASSERT_TRUE(upper.intersects(lower));
}

TEST(RectIntersects, Disjoint)
{
  const Rect lower{Rect<float>::corners(Vec2f{-1.f, -2.f}, Vec2f{3.f, 4.f})};
  const Rect upper{Rect<float>::corners(Vec2f{5.f, 5.f}, Vec2f{6.f, 6.f})};

  ASSERT_FALSE(lower.intersects(upper));
  ASSERT_FALSE(upper.intersects(lower));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
