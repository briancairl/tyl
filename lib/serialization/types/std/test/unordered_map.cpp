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
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/types/std/string.hpp>
#include <tyl/serialization/types/std/unordered_map.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(UnorderedMap, Empty)
{
  {
    file_ostream ofs{"UnorderedMap.Empty.bin"};
    binary_oarchive oar{ofs};
    std::unordered_map<std::string, int> um;
    ASSERT_NO_THROW(oar << um);
  }

  {
    file_istream ifs{"UnorderedMap.Empty.bin"};
    binary_iarchive iar{ifs};
    std::unordered_map<std::string, int> um;
    ASSERT_NO_THROW(iar >> um);

    ASSERT_TRUE(um.empty());
  }
}

TEST(UnorderedMap, PrimitiveElementValue)
{
  static const float TARGET_VALUE = 123.f;

  {
    file_ostream ofs{"UnorderedMap.PrimitiveElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::unordered_map<std::string, float> um = {{"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW(oar << um);
  }

  {
    file_istream ifs{"UnorderedMap.PrimitiveElementValue.bin"};
    binary_iarchive iar{ifs};
    std::unordered_map<std::string, float> um;
    ASSERT_NO_THROW(iar >> um);

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

  {
    file_ostream ofs{"UnorderedMap.TrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::unordered_map<std::string, Trivial> um = {{"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW(oar << um);
  }

  {
    file_istream ifs{"UnorderedMap.TrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::unordered_map<std::string, Trivial> um;
    ASSERT_NO_THROW(iar >> um);

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

  {
    file_ostream ofs{"UnorderedMap.NonTrivialElementValue.bin"};
    binary_oarchive oar{ofs};
    const std::unordered_map<std::string, NonTrivial> um = {
      {"a", TARGET_VALUE}, {"b", TARGET_VALUE}, {"c", TARGET_VALUE}};
    ASSERT_NO_THROW(oar << um);
  }

  {
    file_istream ifs{"UnorderedMap.NonTrivialElementValue.bin"};
    binary_iarchive iar{ifs};
    std::unordered_map<std::string, NonTrivial> um;
    ASSERT_NO_THROW(iar >> um);

    ASSERT_EQ(um.size(), 3UL);
    for (const auto& [key, value] : um)
    {
      ASSERT_EQ(value, TARGET_VALUE);
    }
  }
}
