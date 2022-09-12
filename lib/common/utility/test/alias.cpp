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
#include <tyl/serial/utility/alias.hpp>
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>
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

using namespace tyl::serialization;

TEST(Alias, WriteThenRead)
{
  using LikeAFloat = alias<float, decltype("LikeAFloat"_tag)>;

  mem_ostream oms;

  LikeAFloat w_alias{-0.1234f};

  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW(oar << w_alias);
  }

  {
    mem_istream ims{std::move(oms)};
    binary_iarchive iar{ims};
    LikeAFloat r_alias;
    ASSERT_NO_THROW(iar >> r_alias);
    ASSERT_EQ(w_alias, r_alias);
  }
}
