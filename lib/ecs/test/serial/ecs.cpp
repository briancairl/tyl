/**
 * @copyright 2022-present Brian Cairl
 *
 * @file serialization.cpp
 */

// C++ Standard Library
#include <vector>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serial/ecs/reader.hpp>
#include <tyl/serial/ecs/writer.hpp>
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>

using namespace tyl::serialization;

struct TestComponent
{
  int x;
  float y;
  double z;
};

namespace tyl::serialization
{

template <typename Archive> struct serialize<Archive, ::TestComponent>
{
  void operator()(Archive& ar, ::TestComponent& target)
  {
    ar& named{"x", target.x};
    ar& named{"y", target.y};
    ar& named{"z", target.z};
  }
};

}  // namespace tyl::serialization


TEST(Registry, SaveLoadEmpty)
{
  tyl::ecs::registry saved_reg;
  {
    file_ostream ofs{"Registry.SaveLoadEmpty.json"};
    json_oarchive oar{ofs};
    oar << named{"registry", tyl::ecs::writer<tyl::ecs::entity, float, TestComponent>{saved_reg}};
  }

  {
    tyl::ecs::registry loaded_reg;
    file_istream ifs{"Registry.SaveLoadEmpty.json"};
    json_iarchive iar{ifs};
    tyl::ecs::reader<tyl::ecs::entity, float, TestComponent> reader{loaded_reg};
    iar >> named{"registry", reader};

    ASSERT_EQ(loaded_reg.released(), saved_reg.released());
    ASSERT_EQ(loaded_reg.size(), saved_reg.size());
  }
}

TEST(Registry, SaveLoadNonEmpty)
{
  tyl::ecs::registry saved_reg;

  {
    auto e = saved_reg.create();
    saved_reg.emplace<TestComponent>(e, 1, 2.f, 3.0);
  }

  {
    auto e = saved_reg.create();
    saved_reg.emplace<TestComponent>(e, 1, 2.f, 3.0);
  }

  {
    auto e = saved_reg.create();
    saved_reg.emplace<float>(e, 2.f);
    saved_reg.emplace<TestComponent>(e, 3, 2.f, 1.0);
  }

  {
    file_ostream ofs{"Registry.SaveLoadNonEmpty.bin"};
    binary_oarchive oar{ofs};
    oar << named{"registry", tyl::ecs::writer<tyl::ecs::entity, float, TestComponent>{saved_reg}};
  }

  {
    tyl::ecs::registry loaded_reg;
    file_istream ifs{"Registry.SaveLoadNonEmpty.bin"};
    binary_iarchive iar{ifs};

    tyl::ecs::reader<tyl::ecs::entity, float, TestComponent> reader{loaded_reg};
    iar >> named{"registry", reader};

    ASSERT_EQ(loaded_reg.released(), saved_reg.released());
    ASSERT_EQ(loaded_reg.size(), saved_reg.size());

    auto view = loaded_reg.view<TestComponent>();
    view.each([&loaded_reg, &saved_reg](const auto e, const TestComponent& target) {
      ASSERT_TRUE(saved_reg.valid(e));
      const auto& query = saved_reg.get<TestComponent>(e);
      ASSERT_EQ(query.x, target.x);
      ASSERT_EQ(query.y, target.y);
      ASSERT_EQ(query.z, target.z);
    });
  }
}
