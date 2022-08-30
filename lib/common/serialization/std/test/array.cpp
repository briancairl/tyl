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
#include <tyl/common/serialization/std/array.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;


TEST(Array, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::array<float, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::array<float, 3> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}


TEST(Array, TrivialElementValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::array<Trivial, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::array<Trivial, 3> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}

TEST(Array, NonTrivialElementValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::array<NonTrivial, 3> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::array<NonTrivial, 3> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}
