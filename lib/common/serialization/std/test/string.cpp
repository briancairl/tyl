/**
 * @copyright 2022-present Brian Cairl
 *
 * @file string.cpp
 */

// C++ Standard Library
#include <string>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/common/serialization/std/string.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

TEST(String, Empty)
{
  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::string str;
    ASSERT_NO_THROW((oar << named{"str", str}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::string str;
    ASSERT_NO_THROW((iar >> named{"str", str}));

    ASSERT_TRUE(str.empty());
  }
}

TEST(String, NonEmpty)
{
  mem_ostream oms;
  {
    json_oarchive oar{oms};
    std::string str{"ey boss"};
    ASSERT_NO_THROW((oar << named{"str", str}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};
    std::string str;
    ASSERT_NO_THROW((iar >> named{"str", str}));

    ASSERT_EQ(str, "ey boss");
  }
}
