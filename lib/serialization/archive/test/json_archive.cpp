/**
 * @copyright 2022-present Brian Cairl
 *
 * @file json_archive.hpp
 */

// C++ Standard Library
#include <cstring>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/types/std/vector.hpp>

using namespace tyl::serialization;

struct TrivialStruct
{
  int x;
  float y;
  double z;
};

static bool operator==(const TrivialStruct& lhs, const TrivialStruct& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(TrivialStruct)) == 0;
}

struct TrivialNestedStruct
{
  std::string label_1;
  std::string label_2;
  TrivialStruct first;
  TrivialStruct second;
};

static bool operator==(const TrivialNestedStruct& lhs, const TrivialNestedStruct& rhs)
{
  return (lhs.label_1 == rhs.label_1) and (lhs.label_2 == rhs.label_2) and (lhs.first == rhs.first) and
    (lhs.second == rhs.second);
}

namespace tyl::serialization
{

template <typename Archive> struct serialize<Archive, ::TrivialStruct>
{
  void operator()(Archive& ar, ::TrivialStruct& value)
  {
    ar& named{"x", value.x};
    ar& named{"y", value.y};
    ar& named{"z", value.z};
  }
};

template <typename Archive> struct serialize<Archive, ::TrivialNestedStruct>
{
  void operator()(Archive& ar, ::TrivialNestedStruct& value)
  {
    ar& named{"label_1", value.label_1};
    ar& named{"label_2", value.label_2};
    ar& named{"first", value.first};
    ar& named{"second", value.second};
  }
};

}  // namespace tyl::serialization

TEST(JSONOArchive, Primitive)
{
  file_handle_ostream ofs{stdout};
  json_oarchive oar{ofs};

  // oar << 0.1f;
  ASSERT_NO_THROW((oar << named{"primitive", 0.1f}));
}

TEST(JSONOArchive, TrivialStruct)
{
  file_handle_ostream ofs{stdout};
  json_oarchive oar{ofs};

  TrivialStruct trivial = {5, 123.f, 321.0};
  ASSERT_NO_THROW((oar << named{"trivial", trivial}));
}

TEST(JSONOArchive, TrivialNestedStruct)
{
  file_handle_ostream ofs{stdout};
  json_oarchive oar{ofs};

  TrivialNestedStruct trivial_nested = {"not", "cool", {5, 123.f, 321.0}, {99, 193.f, 1221.0}};
  ASSERT_NO_THROW((oar << named{"trivial_nested", trivial_nested}));
}

TEST(JSONOArchive, ArrayOfPrimitives)
{
  file_handle_ostream ofs{stdout};
  json_oarchive oar{ofs};

  std::vector<float> primitive_array{1.f, 2.f, 3.f, 4.f, 5.f};
  ASSERT_NO_THROW((oar << named{"array", primitive_array}));
}

TEST(JSONOArchive, ArrayOfTrivialStructs)
{
  file_handle_ostream ofs{stdout};
  json_oarchive oar{ofs};

  TrivialStruct element{5, 123.f, 321.0};
  std::vector<TrivialStruct> primitive_array{element, element, element};
  ASSERT_NO_THROW((oar << named{"array", primitive_array}));
}


TEST(JSONIArchive, Primitive)
{
  const float target = 0.1f;

  {
    file_ostream ofs{"Primitive.json"};
    json_oarchive oar{ofs};
    ASSERT_NO_THROW((oar << named{"primitive", target}));
  }

  {
    file_istream ifs{"Primitive.json"};
    json_iarchive iar{ifs};
    float read_value;
    ASSERT_NO_THROW((iar >> named{"primitive", read_value}));
    ASSERT_EQ(target, read_value);
  }
}


TEST(JSONIArchive, TrivialStruct)
{
  const TrivialStruct target = {5, 123.f, 321.0};

  {
    file_ostream ofs{"TrivialStruct.json"};
    json_oarchive oar{ofs};
    ASSERT_NO_THROW((oar << named{"trivial", target}));
  }

  {
    file_istream ifs{"TrivialStruct.json"};
    json_iarchive iar{ifs};
    TrivialStruct read_value;
    ASSERT_NO_THROW((iar >> named{"trivial", read_value}));

    ASSERT_EQ(target, read_value);
  }
}


TEST(JSONIArchive, TrivialNestedStruct)
{
  const TrivialNestedStruct target = {"not", "    cool", {5, 123.f, 321.0}, {99, 193.f, 1221.0}};
  ;

  {
    file_ostream ofs{"TrivialNestedStruct.json"};
    json_oarchive oar{ofs};
    ASSERT_NO_THROW((oar << named{"trivial_nested", target}));
  }

  {
    file_istream ifs{"TrivialNestedStruct.json"};
    json_iarchive iar{ifs};
    TrivialNestedStruct read_value;
    ASSERT_NO_THROW((iar >> named{"trivial_nested", read_value}));

    ASSERT_EQ(target, read_value);
  }
}

TEST(JSONIArchive, ArrayOfPrimitives)
{
  const std::vector<float> target = {1.f, 2.f, 3.f, 4.f, 5.f};

  {
    file_ostream ofs{"ArrayOfPrimitives.json"};
    json_oarchive oar{ofs};
    ASSERT_NO_THROW((oar << named{"array", target}));
  }

  {
    file_istream ifs{"ArrayOfPrimitives.json"};
    json_iarchive iar{ifs};
    std::vector<float> read_value;
    ASSERT_NO_THROW((iar >> named{"array", read_value}));

    ASSERT_EQ(target, read_value);
  }
}

TEST(JSONIArchive, ArrayOfTrivialStructs)
{
  const TrivialStruct target_element{5, 123.f, 321.0};
  const std::vector<TrivialStruct> target = {target_element, target_element, target_element};

  {
    file_ostream ofs{"ArrayOfTrivialStructs.json"};
    json_oarchive oar{ofs};
    ASSERT_NO_THROW((oar << named{"array", target}));
  }

  {
    file_istream ifs{"ArrayOfTrivialStructs.json"};
    json_iarchive iar{ifs};
    std::vector<TrivialStruct> read_value;
    ASSERT_NO_THROW((iar >> named{"array", read_value}));

    ASSERT_EQ(target, read_value);
  }
}
