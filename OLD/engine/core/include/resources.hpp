/**
 * @copyright 2023-present Brian Cairl
 *
 * @file resources.hpp
 */
#pragma once

// Entt
#include <entt/entt.hpp>

// Tyl
#include <tyl/async/worker_pool.hpp>
#include <tyl/engine/core/registry.hpp>

namespace tyl::engine::core
{

/**
 * @brief Top-level handle to active game resources
 */
struct Resources
{
  /// Registry containing all game state and assets
  Registry registry;

  /// Worker pool for deferred work execution
  async::worker_pool worker_pool;
};

}  // namespace tyl::engine::core
