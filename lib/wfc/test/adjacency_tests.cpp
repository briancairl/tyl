/**
 * @copyright 2021 Tyl
 * @author Brian Cairl
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/adjacency.hpp>

using namespace tyl;
using namespace tyl::wfc::adjacency;


TEST(Adjacency, OppositeDirections)
{
  ASSERT_EQ(opposite(Direction::Up), Direction::Down);
  ASSERT_EQ(opposite(Direction::Down), Direction::Up);

  ASSERT_EQ(opposite(Direction::Left), Direction::Right);
  ASSERT_EQ(opposite(Direction::Right), Direction::Left);

  ASSERT_EQ(opposite(Direction::Above), Direction::Below);
  ASSERT_EQ(opposite(Direction::Below), Direction::Above);
}


TEST(AdjacencyTable, InitialState)
{
  static constexpr std::size_t ELEMENT_COUNT = 10;

  Table table{ELEMENT_COUNT};

  ASSERT_EQ(table.element_count(), ELEMENT_COUNT);

  ASSERT_EQ(table.size(), ELEMENT_COUNT * ELEMENT_COUNT);

  for (const auto& adj : table)
  {
    ASSERT_FALSE(bitops::any(adj));
  }
}


TEST(AdjacencyTable, Toggling)
{
  static constexpr std::size_t ELEMENT_COUNT = 10;

  Table table{ELEMENT_COUNT};

  table.allow(0, 1, Direction::Up);

  ASSERT_TRUE(bitops::check(table(0, 1), Direction::Up));
  ASSERT_EQ(bitops::count(table(0, 1)), 1UL);

  ASSERT_FALSE(bitops::check(table(1, 0), Direction::Up));
  ASSERT_FALSE(bitops::check(table(0, 1), Direction::Down));

  table.prevent(0, 1, Direction::Up);

  ASSERT_FALSE(bitops::check(table(0, 1), Direction::Up));
}


TEST(AdjacencyTable, TogglingSymmetric)
{
  static constexpr std::size_t ELEMENT_COUNT = 10;

  Table table{ELEMENT_COUNT};

  table.allow_symmetric(0, 1, Direction::Up);

  ASSERT_TRUE(bitops::check(table(0, 1), Direction::Up));
  ASSERT_EQ(bitops::count(table(0, 1)), 1UL);

  ASSERT_TRUE(bitops::check(table(1, 0), Direction::Down));
  ASSERT_EQ(bitops::count(table(1, 0)), 1UL);

  ASSERT_FALSE(bitops::check(table(1, 0), Direction::Up));
  ASSERT_FALSE(bitops::check(table(0, 1), Direction::Down));

  table.prevent_symmetric(1, 0, Direction::Up);

  ASSERT_TRUE(bitops::check(table(0, 1), Direction::Up));
  ASSERT_TRUE(bitops::check(table(1, 0), Direction::Down));

  table.prevent_symmetric(0, 1, Direction::Up);

  ASSERT_FALSE(bitops::check(table(0, 1), Direction::Up));
  ASSERT_FALSE(bitops::check(table(1, 0), Direction::Down));
}
