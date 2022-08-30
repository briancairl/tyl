/**
 * @copyright 2022-present Brian Cairl
 *
 * @file spritesheet.cpp
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/graphics/serialization/sprite/spritesheet.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

TEST(SpriteSheet, WriteThenRead)
{
  mem_ostream oms;

  tyl::graphics::sprite::SpriteSheet w_spritesheet;
  w_spritesheet.update(tyl::Rect2f{{0.1, 0.2}, {0.3, 0.4}});
  w_spritesheet.update(tyl::Rect2f{{0.1, 0.2}, {0.3, 0.4}});

  {
    json_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"spritesheet", w_spritesheet}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};

    tyl::graphics::sprite::SpriteSheet r_spritesheet;
    ASSERT_NO_THROW((iar >> named{"spritesheet", r_spritesheet}));

    ASSERT_EQ(r_spritesheet.bounds(), w_spritesheet.bounds());
  }
}
