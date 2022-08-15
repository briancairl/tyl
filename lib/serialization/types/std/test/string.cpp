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
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/types/std/string.hpp>

using namespace tyl::serialization;

TEST(String, Empty)
{
  {
    file_ostream ofs{"String.Empty.bin"};
    binary_oarchive oar{ofs};
    std::string str;
    ASSERT_NO_THROW(oar << str);
  }

  {
    file_istream ifs{"String.Empty.bin"};
    binary_iarchive iar{ifs};
    std::string str;
    ASSERT_NO_THROW(iar >> str);

    ASSERT_TRUE(str.empty());
  }
}

TEST(String, NonEmpty)
{
  {
    file_ostream ofs{"String.Empty.bin"};
    binary_oarchive oar{ofs};
    std::string str{"ey boss"};
    ASSERT_NO_THROW(oar << str);
  }

  {
    file_istream ifs{"String.Empty.bin"};
    binary_iarchive iar{ifs};
    std::string str;
    ASSERT_NO_THROW(iar >> str);

    ASSERT_EQ(str, "ey boss");
  }
}
