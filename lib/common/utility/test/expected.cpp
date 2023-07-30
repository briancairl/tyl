/**
 * @copyright 2023-present Brian Cairl
 *
 * @file expected.cpp
 */

// C++ Standard Library
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/utility/expected.hpp>

using namespace tyl;

namespace
{

enum class Error
{
  DID_NOT_WORK,
  DID_NOT_LOAD,
};

}  // namespace

TEST(Expected, ConstructWithValue)
{
  expected<int, Error> e{1};

  ASSERT_TRUE(e);
}

TEST(Expected, ConstructWithError)
{
  expected<int, Error> e{unexpected{Error::DID_NOT_WORK}};

  ASSERT_FALSE(e);
}
