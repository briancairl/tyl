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

TEST(WorkerPool, EmplaceAndDtor)
{
  worker_pool wp{5UL};

  wp.emplace([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
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

  ASSERT_EQ(tracker.get(), 1);
}
