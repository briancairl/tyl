/**
 * @copyright 2022-present Brian Cairl
 *
 * @file optional.cpp
 */

// C++ Standard Library
#include <optional>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serial/std/optional.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(Optional, Empty)
{
  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::optional<int> opt;
    ASSERT_NO_THROW((oar << named{"opt", opt}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::optional<int> opt;
    ASSERT_NO_THROW((iar >> named{"opt", opt}));

    ASSERT_FALSE(opt.has_value());
  }
}

TEST(Optional, PrimitiveValue)
{
  static const float TARGET_VALUE = 123.f;

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::optional<float> opt = TARGET_VALUE;
    ASSERT_NO_THROW((oar << named{"opt", opt}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::optional<float> opt;
    ASSERT_NO_THROW((iar >> named{"opt", opt}));

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}

TEST(Optional, TrivialValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::optional<Trivial> opt = TARGET_VALUE;
    ASSERT_NO_THROW((oar << named{"opt", opt}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::optional<Trivial> opt;
    ASSERT_NO_THROW((iar >> named{"opt", opt}));

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}

TEST(Optional, NonTrivialValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::optional<NonTrivial> opt = TARGET_VALUE;
    ASSERT_NO_THROW((oar << named{"opt", opt}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::optional<NonTrivial> opt;
    ASSERT_NO_THROW((iar >> named{"opt", opt}));

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}
