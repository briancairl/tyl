/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_archive.hpp
 */

// C++ Standard Library
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>

using namespace tyl::serialization;

struct TrivialStruct
{
  int x;
  float y;
  double z;
};

struct NonTrivialStruct
{
  std::vector<int> values;
};

namespace tyl::serialization
{

template <typename OArchive> struct save<OArchive, ::NonTrivialStruct>
{
  void operator()(OArchive& ar, const ::NonTrivialStruct& target)
  {
    ar << target.values.size();
    for (auto& v : target.values)
    {
      ar << v;
    }
  }
};

template <typename IArchive> struct load<IArchive, ::NonTrivialStruct>
{
  void operator()(IArchive& ar, ::NonTrivialStruct& target)
  {
    std::size_t len;
    ar >> len;

    target.values.resize(len);
    for (auto& v : target.values)
    {
      ar >> v;
    }
  }
};

}  // namespace tyl::serialization

TEST(BinaryOArchive, PrimitiveValue)
{
  file_ostream ofs{"text.bin"};
  binary_oarchive oar{ofs};

  float primitive = 123.f;
  ASSERT_NO_THROW(oar << primitive);
}

TEST(BinaryOArchive, TrivialValue)
{
  file_ostream ofs{"text.bin"};
  binary_oarchive oar{ofs};

  TrivialStruct trivial_value;
  ASSERT_NO_THROW(oar << trivial_value);
}

TEST(BinaryOArchive, NonTrivialStruct)
{
  file_ostream ofs{"text.bin"};
  binary_oarchive oar{ofs};

  NonTrivialStruct non_trivial_value;
  ASSERT_NO_THROW(oar << non_trivial_value);
}


TEST(BinaryIArchive, ReadbackTrivialStruct)
{
  const TrivialStruct target_trivial_value{1, 2, 3};

  {
    file_ostream ofs{"text.bin"};
    binary_oarchive oar{ofs};
    ASSERT_NO_THROW(oar << target_trivial_value);
  }

  {
    file_istream ifs{"text.bin"};
    binary_iarchive iar{ifs};

    TrivialStruct read_trivial_value;
    ASSERT_NO_THROW(iar >> read_trivial_value);

    ASSERT_EQ(read_trivial_value.x, target_trivial_value.x);
    ASSERT_EQ(read_trivial_value.y, target_trivial_value.y);
    ASSERT_EQ(read_trivial_value.z, target_trivial_value.z);
  }
}

TEST(BinaryIArchive, ReadbackNonTrivialStruct)
{
  const NonTrivialStruct target_non_trivial_value{{1, 2, 3}};

  ASSERT_GT(target_non_trivial_value.values.size(), 0UL);

  {
    file_ostream ofs{"text.bin"};
    binary_oarchive oar{ofs};
    ASSERT_NO_THROW(oar << target_non_trivial_value);
  }

  {
    file_istream ifs{"text.bin"};
    binary_iarchive iar{ifs};

    NonTrivialStruct read_non_trivial_value;
    ASSERT_NO_THROW(iar >> read_non_trivial_value);

    ASSERT_EQ(read_non_trivial_value.values, target_non_trivial_value.values);
  }
}
