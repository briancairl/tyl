/**
 * @copyright 2022-present Brian Cairl
 *
 * @file meta.cpp
 */

// C++ Standard Library
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/meta.hpp>

using namespace tyl::meta;

template <typename... Ts> struct Pack
{};

TEST(Append, SingleType)
{
  using Expected = Pack<int, float, char>;
  using Appended = append_t<Pack<int, float>, char>;
  EXPECT_TRUE((std::is_same_v<Expected, Appended>));
}

TEST(Append, OtherPack)
{
  using Expected = Pack<int, float, char, int>;
  using Appended = append_t<Pack<int, float>, Pack<char, int>>;
  EXPECT_TRUE((std::is_same_v<Expected, Appended>));
}

TEST(Includes, Empty) { EXPECT_FALSE((includes_v<int>)); }

TEST(Includes, Single)
{
  EXPECT_TRUE((includes_v<int, int>));
  EXPECT_FALSE((includes_v<int, float>));
}

TEST(Includes, Multiple)
{
  EXPECT_TRUE((includes_v<int, int, char>));
  EXPECT_FALSE((includes_v<int, float, char>));
}

TEST(PackIncludes, Empty) { EXPECT_FALSE((pack_includes_v<int, Pack<>>)); }

TEST(PackIncludes, Single)
{
  EXPECT_TRUE((pack_includes_v<int, Pack<int>>));
  EXPECT_FALSE((pack_includes_v<int, Pack<float>>));
}

TEST(PackIncludes, Multiple)
{
  EXPECT_TRUE((pack_includes_v<int, Pack<int, char>>));
  EXPECT_FALSE((pack_includes_v<int, Pack<float, char>>));
}

TEST(Unique, Empty) { EXPECT_TRUE((std::is_same_v<unique_t<Pack<>>, Pack<>>)); }

TEST(Unique, Single) { EXPECT_TRUE((std::is_same_v<unique_t<Pack<int>>, Pack<int>>)); }

TEST(Unique, DuplicateSingle) { EXPECT_TRUE((std::is_same_v<unique_t<Pack<int, int>>, Pack<int>>)); }

TEST(Unique, DuplicateMulti)
{
  EXPECT_TRUE((std::is_same_v<unique_t<Pack<int, int, float, float, char, char>>, Pack<int, float, char>>));
}

TEST(CombineUnique, Empty) { EXPECT_TRUE((std::is_same_v<combine_unique_t<Pack<>, Pack<>>, Pack<>>)); }

TEST(CombineUnique, Single) { EXPECT_TRUE((std::is_same_v<combine_unique_t<Pack<int>, Pack<>>, Pack<int>>)); }

TEST(CombineUnique, DuplicateSingle)
{
  EXPECT_TRUE((std::is_same_v<combine_unique_t<Pack<int>, Pack<int>>, Pack<int>>));
}

TEST(CombineUnique, DuplicateMulti)
{
  EXPECT_TRUE(
    (std::is_same_v<combine_unique_t<Pack<int, int, float>, Pack<float, char, char>>, Pack<int, float, char>>));
}
