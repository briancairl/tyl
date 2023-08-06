/**
 * @copyright 2023-present Brian Cairl
 *
 * @file worker_pool.cpp
 */

// C++ Standard Library
#include <chrono>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/async/worker_pool.hpp>

using namespace tyl::async;


template <typename T> class WorkerPoolTestSuite : public ::testing::Test
{
public:
};

using WorkerPoolTestSuiteTypes = ::testing::Types<worker, static_worker_pool<4>, worker_pool>;

TYPED_TEST_SUITE(WorkerPoolTestSuite, WorkerPoolTestSuiteTypes);

TYPED_TEST(WorkerPoolTestSuite, EmplaceAndDtor)
{
  using worker_pool_type = TypeParam;

  worker_pool_type wp;

  wp.emplace([] { std::this_thread::sleep_for(std::chrono::milliseconds(1)); });
}

TYPED_TEST(WorkerPoolTestSuite, Post)
{
  using worker_pool_type = TypeParam;

  worker_pool_type wp;

  auto tracker = post_nonblocking(wp, [] { return 1; });

  while (true)
  {
    if (auto result_or_error = tracker.get(); result_or_error.has_value())
    {
      ASSERT_EQ(*result_or_error, 1);
      break;
    }
    else
    {
      ASSERT_EQ(result_or_error.error(), non_blocking_future_error::not_ready);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TYPED_TEST(WorkerPoolTestSuite, PostVoid)
{
  using worker_pool_type = TypeParam;

  worker_pool_type wp;

  auto tracker = post_nonblocking(wp, [] {});

  while (true)
  {
    if (auto result_or_error = tracker.get(); result_or_error.has_value())
    {
      break;
    }
    else
    {
      ASSERT_EQ(result_or_error.error(), non_blocking_future_error::not_ready);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TYPED_TEST(WorkerPoolTestSuite, PostBlocking)
{
  using worker_pool_type = TypeParam;

  worker_pool_type wp;

  auto tracker = post_blocking(wp, [] { return 1; });

  ASSERT_EQ(tracker.get(), 1);
}
