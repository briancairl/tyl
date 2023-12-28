/**
 * @copyright 2023-present Brian Cairl
 *
 * @file execution.hpp
 */
#pragma once

// Tyl
#include <tyl/async.hpp>
#include <tyl/clock.hpp>

namespace tyl::engine
{

/**
 * @brief Shared execution resources
 */
struct SharedState
{
  /// Current time
  Clock::Time now;

  /// Thread pool for deferred work execution
  async::ThreadPool thread_pool;
};

}  // namespace tyl::engine
