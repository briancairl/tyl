/**
 * @copyright 2022-present Brian Cairl
 *
 * @file bitfield.cpp
 */

// C++ Standard Library
#include <cstdint>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/utility/bitfield.hpp>

using namespace tyl;

struct BitFieldSingleBlock : bitfield<BitFieldSingleBlock, std::uint64_t, 1>
{
  std::uint64_t a : 1;
  std::uint64_t b : 2;
  std::uint64_t c : 1;
};

struct BitFieldMultiBlock : bitfield<BitFieldMultiBlock, std::uint64_t, 2>
{
  std::uint64_t a : 16;
  std::uint64_t b : 16;
  std::uint64_t c : 16;
  std::uint64_t d : 16;
  std::uint64_t e : 16;
  std::uint64_t f : 16;
  std::uint64_t g : 16;
  std::uint64_t h : 16;
};

TEST(BitFieldSingleBlock, SetAndClear)
{
  BitFieldSingleBlock bf{.a = 0, .b = 0, .c = 0};

  ASSERT_FALSE(bf.a) << bf.block<0>();
  bf.set<0>();
  EXPECT_TRUE(bf.a) << bf.block<0>();
  bf.clear<0>();
  EXPECT_FALSE(bf.a) << bf.block<0>();

  ASSERT_FALSE(bf.b) << bf.block<0>();
  bf.set<1>();
  EXPECT_TRUE(bf.b) << bf.block<0>();
  bf.clear<1>();
  EXPECT_FALSE(bf.b) << bf.block<0>();
}

TEST(BitFieldSingleBlock, SetTo)
{
  BitFieldSingleBlock bf{.a = 0, .b = 0, .c = 0};

  bf.set_to<0>(true);
  EXPECT_TRUE(bf.a) << bf.block<0>();

  bf.set_to<0>(false);
  EXPECT_FALSE(bf.a) << bf.block<0>();
}

TEST(BitFieldSingleBlock, BitwiseOr)
{
  const BitFieldSingleBlock bf_a{.a = 1, .b = 0, .c = 1};
  const BitFieldSingleBlock bf_b{.a = 0, .b = 1, .c = 0};

  const auto bf_c = bf_a | bf_b;

  EXPECT_TRUE(bf_c.a) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_TRUE(bf_c.b) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_TRUE(bf_c.c) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
}

TEST(BitFieldSingleBlock, BitwiseAnd)
{
  const BitFieldSingleBlock bf_a{.a = 1, .b = 0, .c = 1};
  const BitFieldSingleBlock bf_b{.a = 0, .b = 1, .c = 0};

  const auto bf_c = bf_a & bf_b;

  EXPECT_FALSE(bf_c.a) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_FALSE(bf_c.b) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_FALSE(bf_c.c) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
}

TEST(BitFieldSingleBlock, Any)
{
  const BitFieldSingleBlock bf_a{.a = 1, .b = 0, .c = 1};
  EXPECT_TRUE(bf_a.any()) << bf_a.block<0>();
  const BitFieldSingleBlock bf_b{.a = 0, .b = 0, .c = 0};
  EXPECT_FALSE(bf_b.any()) << bf_b.block<0>();
}

TEST(BitFieldSingleBlock, None)
{
  const BitFieldSingleBlock bf_a{.a = 1, .b = 0, .c = 1};
  EXPECT_FALSE(bf_a.none()) << bf_a.block<0>();
  const BitFieldSingleBlock bf_b{.a = 0, .b = 0, .c = 0};
  EXPECT_TRUE(bf_b.none()) << bf_b.block<0>();
}

TEST(BitFieldMultiBlock, BitwiseOr)
{
  const BitFieldMultiBlock bf_a{.a = 1, .b = 0, .c = 1, .f = 1};
  const BitFieldMultiBlock bf_b{.a = 0, .b = 1, .c = 0, .f = 0};

  const auto bf_c = bf_a | bf_b;

  EXPECT_TRUE(bf_c.a) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_TRUE(bf_c.b) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_TRUE(bf_c.c) << bf_a.block<0>() << " | " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_TRUE(bf_c.f) << bf_a.block<1>() << " | " << bf_b.block<1>() << " => " << bf_c.block<1>();
}

TEST(BitFieldMultiBlock, BitwiseAnd)
{
  const BitFieldMultiBlock bf_a{.a = 1, .b = 0, .c = 1, .f = 1};
  const BitFieldMultiBlock bf_b{.a = 0, .b = 1, .c = 0, .f = 0};

  const auto bf_c = bf_a & bf_b;

  EXPECT_FALSE(bf_c.a) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_FALSE(bf_c.b) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_FALSE(bf_c.c) << bf_a.block<0>() << " & " << bf_b.block<0>() << " => " << bf_c.block<0>();
  EXPECT_FALSE(bf_c.f) << bf_a.block<1>() << " & " << bf_b.block<1>() << " => " << bf_c.block<1>();
}

TEST(BitFieldMultiBlock, Any)
{
  const BitFieldMultiBlock bf_a{.a = 1, .b = 0, .c = 1};
  EXPECT_TRUE(bf_a.any()) << bf_a.block<1>();
  const BitFieldMultiBlock bf_b{.a = 0, .b = 0, .c = 0};
  EXPECT_FALSE(bf_b.any()) << bf_b.block<1>();
}

TEST(BitFieldMultiBlock, None)
{
  const BitFieldMultiBlock bf_a{.a = 0, .b = 0, .c = 0, .f = 1};
  EXPECT_FALSE(bf_a.none()) << bf_a.block<1>();
  const BitFieldMultiBlock bf_b{.a = 0, .b = 0, .c = 0, .f = 0};
  EXPECT_TRUE(bf_b.none()) << bf_b.block<1>();
}
