/**
 * @copyright 2021 Tyl
 * @author Brian Cairl
 */

// C++ Standard Library
#include <fstream>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization.hpp>
#include <tyl/serialization/archive.hpp>

namespace tyl
{

struct TestObject
{
  int a;
  float b;
};

template <typename ArchiveT>
static void serialize(ArchiveT& ar, TestObject& value, const serialization::version_t version)
{
  serialization::split(ar, value, version);
}

template <typename ArchiveT> static void load(ArchiveT& ar, TestObject& value, const serialization::version_t version)
{
  ar >> serialization::field("a", value.a);
  ar >> serialization::field("b", value.b);
}

template <typename ArchiveT>
static void save(ArchiveT& ar, const TestObject& value, const serialization::version_t version)
{
  ar << serialization::field("a", value.a);
  ar << serialization::field("b", value.b);
}

}  // namespace tyl

TEST(Serialization, Save)
{
  std::ofstream ofs{"serialization_test.bin"};

  tyl::serialization::OArchive oa{ofs};

  const tyl::TestObject object{.a = 1, .b = 0.1f};

  ASSERT_EQ(object.a, 1);
  ASSERT_EQ(object.b, 0.1f);

  oa << tyl::serialization::field("test_object", object);

  ASSERT_EQ(object.a, 1);
  ASSERT_EQ(object.b, 0.1f);
}

TEST(Serialization, Load)
{
  std::ifstream ifs{"serialization_test.bin"};

  tyl::serialization::IArchive ia{ifs};

  tyl::TestObject object{};

  ASSERT_EQ(object.a, 0);
  ASSERT_EQ(object.b, 0);

  ia >> tyl::serialization::field("test_object", object);

  ASSERT_EQ(object.a, 1);
  ASSERT_EQ(object.b, 0.1f);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
