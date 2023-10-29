/**
 * @copyright 2023-present Brian Cairl
 *
 * @file optional.cpp
 */

// C++ Standard Library
#include <optional>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/mem_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/std/optional.hpp>
#include <tyl/serialization/std/string.hpp>

using namespace tyl::serialization;


TEST(StdOptional, EmptyOptionalTrivial)
{
  const std::optional<int> kExpected = {};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::optional<int> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdOptional, NonEmptyOptionalTrivial)
{
  const std::optional<int> kExpected{123};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::optional<int> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdOptional, EmptyOptionalNonTrivial)
{
  const std::optional<std::string> kExpected = {};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::optional<std::string> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}

TEST(StdOptional, NonEmptyOptionalNonTrivial)
{
  const std::optional<std::string> kExpected{"hello!"};

  mem_ostream oms{};
  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"value", kExpected}));
  }

  mem_istream ims{std::move(oms)};
  {
    binary_iarchive iar{ims};
    std::optional<std::string> read;
    ASSERT_NO_THROW((iar >> named{"value", read}));
    ASSERT_EQ(read, kExpected);
  }
}
