/**
 * @copyright 2022-present Brian Cairl
 *
 * @file math.cpp
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/serial/math/rect.hpp>
#include <tyl/serial/math/size.hpp>
#include <tyl/serial/math/vec.hpp>
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/mem_istream.hpp>
#include <tyl/serialization/mem_ostream.hpp>

using namespace tyl::serialization;

TEST(Vec, WriteThenRead)
{
  mem_ostream oms;

  tyl::Vec<float, 4> w_vec;
  w_vec << 1.f, 2.f, 3.f, 4.f;

  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW(oar << w_vec);
  }

  {
    mem_istream ims{std::move(oms)};
    binary_iarchive iar{ims};
    tyl::Vec<float, 4> r_vec;
    ASSERT_NO_THROW(iar >> r_vec);

    for (int i = 0; i < 4; ++i)
    {
      ASSERT_NEAR(w_vec[i], r_vec[i], 1e-3f);
    }
  }
}

TEST(Rect, WriteThenRead)
{
  mem_ostream oms;

  tyl::Rect2i w_rect{{1, 2}, {3, 4}};

  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW(oar << w_rect);
  }

  {
    mem_istream ims{std::move(oms)};
    binary_iarchive iar{ims};
    tyl::Rect2i r_rect;
    ASSERT_NO_THROW(iar >> r_rect);
    ASSERT_EQ(w_rect, r_rect);
  }
}

TEST(Size, WriteThenRead)
{
  mem_ostream oms;

  tyl::Size2f w_size{{1, 2}};

  {
    binary_oarchive oar{oms};
    ASSERT_NO_THROW(oar << w_size);
  }

  {
    mem_istream ims{std::move(oms)};
    binary_iarchive iar{ims};
    tyl::Size2f r_size;
    ASSERT_NO_THROW(iar >> r_size);
    ASSERT_EQ(w_size, r_size);
  }
}
