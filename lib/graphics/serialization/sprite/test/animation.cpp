/**
 * @copyright 2022-present Brian Cairl
 *
 * @file animation.cpp
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/graphics/serialization/sprite/animation.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

TEST(AnimationState, WriteThenRead)
{
  mem_ostream oms;

  tyl::graphics::sprite::AnimationState w_state{.complete = true, .progress = 0.4f};

  {
    json_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"state", w_state}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};

    tyl::graphics::sprite::AnimationState r_state;
    ASSERT_NO_THROW((iar >> named{"state", r_state}));

    ASSERT_EQ(r_state.complete, w_state.complete);
    ASSERT_EQ(r_state.progress, w_state.progress);
  }
}
