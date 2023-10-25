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
#include <tyl/clock.hpp>
#include <tyl/ecs.hpp>
#include <tyl/expected.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/vec.hpp>

namespace tyl::engine
{

/**
 * @brief Resources used to update a widget
 */
struct WidgetResources
{
  /// Current time
  Clock::Time now = Clock::Time::min();
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

template <typename IStreamT> using WidgetIArchive = tyl::serialization::binary_iarchive<IStreamT>;

template <typename OStreamT> using WidgetOArchive = tyl::serialization::binary_oarchive<OStreamT>;

}  // namespace tyl::engine
