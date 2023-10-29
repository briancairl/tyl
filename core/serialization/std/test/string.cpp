/**
 * @copyright 2023-present Brian Cairl
 *
 * @file string.cpp
 */

// C++ Standard Library
#include <string>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/string.hpp>

using namespace tyl::serialization;


TEST(StdString, EmptyString)
{
  const std::string kExpected = {};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::string read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdString, NonEmptyString)
{
  const std::string kExpected = "expected";

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::string read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
