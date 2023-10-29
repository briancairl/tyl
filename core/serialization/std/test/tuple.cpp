/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tuple.cpp
 */

// C++ Standard Library
#include <string>
#include <tuple>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/string.hpp>
#include <tyl/serialization/std/tuple.hpp>

using namespace tyl::serialization;


TEST(StdUtility, TrivialTuple)
{
  const std::tuple<int, double> kExpected{1, 1.3};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::tuple<int, double> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdUtility, NonTrivialTuple)
{
  const std::tuple<std::string, double> kExpected{std::to_string(1), 1.3};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::tuple<std::string, double> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
