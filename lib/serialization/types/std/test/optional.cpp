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
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/types/std/optional.hpp>

#include "test_types.hpp"

using namespace tyl::serialization;

TEST(Optional, Empty)
{
  {
    file_ostream ofs{"Optional.Empty.bin"};
    binary_oarchive oar{ofs};
    std::optional<int> opt;
    ASSERT_NO_THROW(oar << opt);
  }

  {
    file_istream ifs{"Optional.Empty.bin"};
    binary_iarchive iar{ifs};
    std::optional<int> opt;
    ASSERT_NO_THROW(iar >> opt);

    ASSERT_FALSE(opt.has_value());
  }
}

TEST(Optional, PrimitiveValue)
{
  static const float TARGET_VALUE = 123.f;

  {
    file_ostream ofs{"Optional.PrimitiveValue.bin"};
    binary_oarchive oar{ofs};
    std::optional<float> opt = TARGET_VALUE;
    ASSERT_NO_THROW(oar << opt);
  }

  {
    file_istream ifs{"Optional.PrimitiveValue.bin"};
    binary_iarchive iar{ifs};
    std::optional<float> opt;
    ASSERT_NO_THROW(iar >> opt);

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}

TEST(Optional, TrivialValue)
{
  static const Trivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Optional.TrivialValue.bin"};
    binary_oarchive oar{ofs};
    std::optional<Trivial> opt = TARGET_VALUE;
    ASSERT_NO_THROW(oar << opt);
  }

  {
    file_istream ifs{"Optional.TrivialValue.bin"};
    binary_iarchive iar{ifs};
    std::optional<Trivial> opt;
    ASSERT_NO_THROW(iar >> opt);

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}

TEST(Optional, NonTrivialValue)
{
  static const NonTrivial TARGET_VALUE = {6, 9};

  {
    file_ostream ofs{"Optional.NonTrivialValue.bin"};
    binary_oarchive oar{ofs};
    std::optional<NonTrivial> opt = TARGET_VALUE;
    ASSERT_NO_THROW(oar << opt);
  }

  {
    file_istream ifs{"Optional.NonTrivialValue.bin"};
    binary_iarchive iar{ifs};
    std::optional<NonTrivial> opt;
    ASSERT_NO_THROW(iar >> opt);

    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), TARGET_VALUE);
  }
}
