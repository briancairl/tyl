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
#include <tyl/serial/ecs/reference.hpp>
#include <tyl/serial/ecs/writer.hpp>
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

struct TestComponent
{
  int x;
  float y;
  double z;
};

constexpr bool operator==(const TestComponent& lhs, const TestComponent& rhs)
{
  return std::memcmp(&lhs, &rhs, sizeof(TestComponent)) == 0;
}

namespace tyl::serialization
{

template <typename OArchive> struct serialize<OArchive, ::TestComponent>
{
  void operator()(OArchive& ar, ::TestComponent& target)
  {
    ar& named{"x", target.x};
    ar& named{"y", target.y};
    ar& named{"z", target.z};
  }
};

}  // namespace tyl::serialization


TEST(Reference, ResolveDefaultReference)
{
  tyl::ecs::registry saved_reg;

  const auto e = saved_reg.create();
  {
    saved_reg.emplace<TestComponent>(e, 1, 2.f, 3.0);

    const auto ref_e = saved_reg.create();
    saved_reg.emplace<tyl::ecs::ref<TestComponent>>(ref_e, e, saved_reg);
  }


  mem_ostream oms;
  {
    binary_oarchive oar{oms};
    oar << named{
      "registry", tyl::ecs::writer<tyl::ecs::entity, TestComponent, tyl::ecs::ref<TestComponent>>{saved_reg}};
  }

  {
    tyl::ecs::registry loaded_reg;
    mem_istream ims{std::move(oms)};
    binary_iarchive iar{ims};
    tyl::ecs::reader<tyl::ecs::entity, TestComponent, tyl::ecs::ref<TestComponent>> reader{loaded_reg};
    iar >> named{"registry", reader};

    ASSERT_EQ(loaded_reg.released(), saved_reg.released());
    ASSERT_EQ(loaded_reg.size(), saved_reg.size());

    tyl::ecs::resolve_references<tyl::ecs::ref<TestComponent>>(loaded_reg);

    ASSERT_TRUE(loaded_reg.all_of<TestComponent>(e));
    ASSERT_EQ(loaded_reg.get<TestComponent>(e), saved_reg.get<TestComponent>(e));
  }
}
