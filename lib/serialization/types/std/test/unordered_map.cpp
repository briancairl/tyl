/**
 * @copyright 2022-present Brian Cairl
 *
 * @file unordered_map.cpp
 */

// C++ Standard Library
#include <unordered_map>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>
#include <tyl/serialization/types/std/string.hpp>
#include <tyl/serialization/types/std/unordered_map.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(UnorderedMap, Empty)
{
  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::unordered_map<std::string, int> um;
    ASSERT_NO_THROW((oar << named{"map", um}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::unordered_map<std::string, int> um;
    ASSERT_NO_THROW((iar >> named{"map", um}));

    ASSERT_TRUE(um.empty());
  }
}

TEST(UnorderedMap, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::unordered_map<std::string, float> um = {{"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW((oar << named{"map", um}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::unordered_map<std::string, float> um;
    ASSERT_NO_THROW((iar >> named{"map", um}));

    ASSERT_EQ(um.size(), 3UL);
    for (const auto& [key, value] : um)
    {
      ASSERT_EQ(value, TARGET_VALUE);
    }
  }
}


TEST(UnorderedMap, TrivialElementValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::unordered_map<std::string, Trivial> um = {{"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW((oar << named{"map", um}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::unordered_map<std::string, Trivial> um;
    ASSERT_NO_THROW((iar >> named{"map", um}));

    ASSERT_EQ(um.size(), 3UL);
    for (const auto& [key, value] : um)
    {
      ASSERT_EQ(value, TARGET_VALUE);
    }
  }
}

TEST(UnorderedMap, NonTrivialElementValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  mem_ostream oms;
  {
    json_oarchive oar{oms};
    const std::unordered_map<std::string, NonTrivial> um = {
      {"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW((oar << named{"map", um}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::unordered_map<std::string, NonTrivial> um;
    ASSERT_NO_THROW((iar >> named{"map", um}));

    ASSERT_EQ(um.size(), 3UL);
    for (const auto& [key, value] : um)
    {
      ASSERT_EQ(value, TARGET_VALUE);
    }
  }
}
