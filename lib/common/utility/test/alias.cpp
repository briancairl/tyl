/**
 * @copyright 2022-present Brian Cairl
 *
 * @file alias.cpp
 */

// C++ Standard Library
#include <string>
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/utility/alias.hpp>

using namespace tyl;

TEST(Alias, PrimitiveEqualityWithUnderlying)
{
  using LikeAnInt = alias<int, decltype("LikeAnInt"_tag)>;

  LikeAnInt i{2};

  EXPECT_EQ(i, int{2});
  EXPECT_EQ(int{2}, i);
}

TEST(Alias, PrimitiveInequalityWithUnderlying)
{
  using LikeAnInt = alias<int, decltype("LikeAnInt"_tag)>;

  LikeAnInt i{2};

  EXPECT_NE(i, int{1});
  EXPECT_NE(int{1}, i);
}


TEST(Alias, ClassEqualityWithUnderlying)
{
  using LikeAString = alias<std::string, decltype("LikeAString"_tag)>;

  LikeAString str{"ok"};

  EXPECT_EQ(str, std::string{"ok"});
  EXPECT_EQ(std::string{"ok"}, str);
}

TEST(Alias, ClassInequalityWithUnderlying)
{
  using LikeAString = alias<std::string, decltype("LikeAString"_tag)>;

  LikeAString str{"ok"};

  EXPECT_NE(str, std::string{"ok1"});
  EXPECT_NE(std::string{"ok1"}, str);
}
