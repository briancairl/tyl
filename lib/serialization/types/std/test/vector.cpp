/**
 * @copyright 2022-present Brian Cairl
 *
 * @file vector.cpp
 */

// C++ Standard Library
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/types/std/vector.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(Vector, Empty)
{
  {
    file_ostream ofs{"Vector.Empty.bin"};
    binary_oarchive oar{ofs};
    std::vector<int> vec;
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Vector.Empty.bin"};
    binary_iarchive iar{ifs};
    std::vector<int> vec;
    ASSERT_NO_THROW(iar >> vec);

    ASSERT_TRUE(vec.empty());
  }
}

TEST(Vector, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  {
    file_ostream ofs{"Vector.PrimitiveElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::vector<float> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Vector.PrimitiveElementValue.bin"};
    binary_iarchive iar{ifs};
    std::vector<float> vec;
    ASSERT_NO_THROW(iar >> vec);

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}


TEST(Vector, TrivialElementValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Vector.TrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::vector<Trivial> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Vector.TrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::vector<Trivial> vec;
    ASSERT_NO_THROW(iar >> vec);

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e.x, TARGET_VALUE.x);
      ASSERT_EQ(e.y, TARGET_VALUE.y);
    }
  }
}

TEST(Vector, NonTrivialElementValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Vector.NonTrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::vector<NonTrivial> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Vector.NonTrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::vector<NonTrivial> vec;
    ASSERT_NO_THROW(iar >> vec);

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e.x, TARGET_VALUE.x);
      ASSERT_EQ(e.y, TARGET_VALUE.y);
    }
  }
}
