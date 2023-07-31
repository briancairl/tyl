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

TEST(Worker, EmplaceAndDtor)
{
  worker wp;

  wp.emplace([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
}

TEST(Worker, Post)
{
  worker wp;

  auto tracker = post(wp, [] { return 1; });

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
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TEST(Worker, PostBlocking)
{
  worker wp;

  auto tracker = post<post_strategy::blocking>(wp, [] { return 1; });

  ASSERT_EQ(tracker.get(), 1);
}

TEST(WorkerPool, Post)
{
  worker_pool wp{5UL};

  auto tracker = post(wp, [] { return 1; });

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
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TEST(WorkerPool, EmplaceAndDtor)
{
  worker_pool wp{5UL};

  wp.emplace([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
}

TEST(WorkerPool, PostBlocking)
{
  worker_pool wp{5UL};

  auto tracker = post<post_strategy::blocking>(wp, [] { return 1; });

  ASSERT_EQ(tracker.get(), 1);
}


TEST(StaticWorkerPool, EmplaceAndDtor)
{
  static_worker_pool<4> wp;

  wp.emplace([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
}

TEST(StaticWorkerPool, Post)
{
  static_worker_pool<4> wp;

  auto tracker = post(wp, [] { return 1; });

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
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TEST(StaticWorkerPool, PostVoid)
{
  static_worker_pool<4> wp;

  auto tracker = post(wp, [] {});

  while (true)
  {
    if (auto result_or_error = tracker.get(); result_or_error.has_value())
    {
      break;
    }
    else
    {
      ASSERT_EQ(result_or_error.error(), non_blocking_future_error::not_ready);
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  ASSERT_EQ(tracker.get(), non_blocking_future_error::retrieved);
}

TEST(StaticWorkerPool, PostBlocking)
{
  static_worker_pool<4> wp;

  auto tracker = post<post_strategy::blocking>(wp, [] { return 1; });

  ASSERT_EQ(tracker.get(), 1);
}
