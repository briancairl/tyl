/**
 * @copyright 2022-present Brian Cairl
 *
 * @file array.cpp
 */

// C++ Standard Library
#include <array>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/types/std/array.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;


TEST(Array, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  {
    file_ostream ofs{"Array.PrimitiveElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::array<float, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Array.PrimitiveElementValue.bin"};
    binary_iarchive iar{ifs};
    std::array<float, 3> vec;
    ASSERT_NO_THROW(iar >> vec);

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}


TEST(Array, TrivialElementValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Array.TrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::array<Trivial, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Array.TrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::array<Trivial, 3> vec;
    ASSERT_NO_THROW(iar >> vec);

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}

TEST(Array, NonTrivialElementValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Array.NonTrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::array<NonTrivial, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW(oar << vec);
  }

  {
    file_istream ifs{"Array.NonTrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::array<NonTrivial, 3> vec;
    ASSERT_NO_THROW(iar >> vec);

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}
