/**
 * @copyright 2023-present Brian Cairl
 *
 * @file common.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <vector>

// Tyl
#include <tyl/async.hpp>
#include <tyl/ecs.hpp>
#include <tyl/expected.hpp>
#include <tyl/vec.hpp>

namespace tyl::engine
{

/**
 * @brief Resources used to update a widget
 */
struct WidgetResources
{
  /// Handle to active engine GUI framework context
  void* gui_context;
  /// Drag-and-drop payloads
  std::vector<std::filesystem::path> drop_payloads = {};
  /// Location at which
  Vec2f drop_cursor_position = Vec2f::Zero();
};

/**
 * @brief Resources used to update a widget
 */
struct WidgetSharedState
{
  /// Thread pool for deferred work execution
  async::ThreadPool thread_pool;
};


/**
 * @brief Errors used on widget creation failures
 */
enum class WidgetCreationError
{

};

/**
 * @brief Statuses used on widget update
 */
enum class WidgetStatus
{
  kOk
};

}  // namespace tyl::engine
