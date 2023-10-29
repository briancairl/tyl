/**
 * @copyright 2023-present Brian Cairl
 *
 * @file utility.cpp
 */

// C++ Standard Library
#include <string>
#include <utility>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/string.hpp>
#include <tyl/serialization/std/utility.hpp>

using namespace tyl::serialization;


TEST(StdUtility, TrivialPair)
{
  const std::pair<int, double> kExpected{1, 1.3};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::pair<int, double> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdUtility, NonTrivialPair)
{
  const std::pair<std::string, double> kExpected{std::to_string(1), 1.3};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::pair<std::string, double> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
