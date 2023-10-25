/**
 * @copyright 2023-present Brian Cairl
 *
 * @file vector.cpp
 */

// C++ Standard Library
#include <string>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/string.hpp>
#include <tyl/serialization/std/vector.hpp>

using namespace tyl::serialization;


TEST(StdVector, TriviallySerializableElement)
{
  const std::vector<float> kExpected = {1, 2, 3, 4, 5};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::vector<float> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdVector, NonTriviallySerializableElement)
{
  const std::vector<std::string> kExpected = {"1", "2", "3", "4", "5"};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::vector<std::string> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
