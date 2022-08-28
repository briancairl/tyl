/**
 * @copyright 2022-present Brian Cairl
 *
 * @file named.cpp
 */

// C++ Standard Library
#include <cstring>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/named.hpp>

using namespace tyl::serialization;


TEST(Named, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  {
    file_ostream ofs{"Named.PrimitiveElementValue.bin"};
    binary_oarchive oar{ofs};
    const float v = TARGET_VALUE;
    ASSERT_NO_THROW((oar << named{"value", v}));
  }

  {
    file_istream ifs{"Named.PrimitiveElementValue.bin"};
    binary_iarchive iar{ifs};
    float v;
    ASSERT_NO_THROW((iar >> named{"value", v}));
    ASSERT_EQ(v, TARGET_VALUE);
  }
}

TEST(Named, TrivialValue)
{
  struct Trivial
  {
    int x;
    float y, z;
  };

  static const Trivial TARGET_VALUE = {1, 123.f, 321.f};

  {
    file_ostream ofs{"Named.TrivialValue.bin"};
    binary_oarchive oar{ofs};
    const Trivial v = TARGET_VALUE;
    ASSERT_NO_THROW((oar << named{"value", v}));
  }

  {
    file_istream ifs{"Named.TrivialValue.bin"};
    binary_iarchive iar{ifs};
    Trivial v;
    ASSERT_NO_THROW((iar >> named{"value", v}));
    ASSERT_EQ(std::memcmp(&v, &TARGET_VALUE, sizeof(TARGET_VALUE)), 0);
  }
}
