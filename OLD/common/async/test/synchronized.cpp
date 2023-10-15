/**
 * @copyright 2023-present Brian Cairl
 *
 * @file synchronized.cpp
 */

// C++ Standard Library
#include <atomic>
#include <chrono>
#include <optional>
#include <string>
#include <thread>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/async/synchronized.hpp>

using namespace tyl::async;

TEST(Synchronized, ReadThenRead)
{
  synchronized<std::string> synced{"value"};

  std::atomic_bool flag = false;

  auto read_handle = std::optional{synced.rlock()};

  std::thread other_thread{[&synced, &flag] {
    synced.with_lock([&flag](const auto& read_value) {
      flag = true;
      ASSERT_EQ(read_value, "value");
    });
  }};

  other_thread.join();
  ASSERT_TRUE(flag);
}

TEST(Synchronized, ReadThenWrite)
{
  synchronized<std::string> synced{"value"};

  std::atomic_bool flag = false;

  auto read_handle = std::optional{synced.rlock()};

  std::thread other_thread{[&synced, &flag] {
    synced.with_lock([&flag](std::string& write_value) {
      flag = true;
      write_value = "new_value";
    });
  }};

  ASSERT_FALSE(flag);
  read_handle.reset();
  other_thread.join();
  ASSERT_TRUE(flag);
}

TEST(Synchronized, WriteThenWrite)
{
  synchronized<std::string> synced{"value"};

  std::atomic_bool flag = false;

  auto write_handle = std::optional{synced.wlock()};

  std::thread other_thread{[&synced, &flag] {
    synced.with_lock([&flag](std::string& write_value) {
      flag = true;
      write_value = "new_value";
    });
  }};

  ASSERT_FALSE(flag);
  write_handle.reset();
  other_thread.join();
  ASSERT_TRUE(flag);
}

TEST(Synchronized, Equality)
{
  synchronized<std::string> synced{"value"};

  ASSERT_EQ(synced, "value");
}
