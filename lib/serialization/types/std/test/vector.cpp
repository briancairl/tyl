/**
 * @copyright 2022-present Brian Cairl
 *
 * @file vector.cpp
 */

// C++ Standard Library
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>
#include <tyl/serialization/types/std/vector.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(Vector, Empty)
{
  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::vector<int> vec;
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::vector<int> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    ASSERT_TRUE(vec.empty());
  }
}

TEST(Vector, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::vector<float> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::vector<float> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e, TARGET_VALUE);
    }
  }
}


TEST(Vector, TrivialElementValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::vector<Trivial> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::vector<Trivial> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e.x, TARGET_VALUE.x);
      ASSERT_EQ(e.y, TARGET_VALUE.y);
    }
  }
}

TEST(Vector, NonTrivialElementValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::vector<NonTrivial> vec = {TARGET_VALUE, TARGET_VALUE, TARGET_VALUE};
    ASSERT_NO_THROW((oar << named{"vec", vec}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::vector<NonTrivial> vec;
    ASSERT_NO_THROW((iar >> named{"vec", vec}));

    ASSERT_EQ(vec.size(), 3UL);
    for (const auto& e : vec)
    {
      ASSERT_EQ(e.x, TARGET_VALUE.x);
      ASSERT_EQ(e.y, TARGET_VALUE.y);
    }
  }
}
