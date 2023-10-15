/**
 * @copyright 2021-present Brian Cairl
 *
 * @file async.hpp
 */
#pragma once

// Parachute
#include <parachute/non_blocking_future.hpp>
#include <parachute/pool.hpp>
#include <parachute/post.hpp>

namespace tyl::async
{

using thread_pool = ::para::pool;

template <typename T> using non_blocking_future = ::para::non_blocking_future;

/**
 * @brief Enqueues work to a work pool and returns a tracker for that work
 */
template <typename PoolT, typename WorkT> [[nodiscard]] decltype(auto) post(PoolT&& pool, WorkT&& work)
{
  return ::para::post<::para::strategy::non_blocking>(std::forward<PoolT>(pool), std::forward<WorkT>(work));
}
/**
 * @brief Enqueues work to a work pool and returns a tracker for that work
 */
template <typename PoolT, typename WorkT> [[nodiscard]] decltype(auto) post_blocking(PoolT&& pool, WorkT&& work)
{
  return ::para::post<::para::strategy::blocking>(std::forward<PoolT>(pool), std::forward<WorkT>(work));
}

}  // namespace tyl::async
