/**
 * @copyright 2022-present Brian Cairl
 *
 * @file image.cpp
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serial/graphics/host/image.hpp>
#include <tyl/serialization/json_iarchive.hpp>
#include <tyl/serialization/json_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

TEST(ImageOptions, WriteThenRead)
{
  mem_ostream oms;

  tyl::graphics::host::ImageOptions w_options{
    .channel_mode = tyl::graphics::host::ImageOptions::ChannelMode::RGBA, .flags = {.flip_vertically = true}};

  {
    json_oarchive oar{oms};
    ASSERT_NO_THROW((oar << named{"options", w_options}));
  }

  {
    mem_istream ims{std::move(oms)};
    json_iarchive iar{ims};

    tyl::graphics::host::ImageOptions r_options;
    ASSERT_NO_THROW((iar >> named{"options", r_options}));

    ASSERT_EQ(r_options.channel_mode, w_options.channel_mode);
    ASSERT_EQ(r_options.flags.flip_vertically, w_options.flags.flip_vertically);
  }
}
