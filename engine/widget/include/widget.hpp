/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <vector>

// Tyl
#include <tyl/async.hpp>
#include <tyl/crtp.hpp>
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

void WidgetUpdateCommon(Registry& registry, WidgetResources& resources);

template <typename WidgetT> struct WidgetOptions;
// {
//   using type = WidgetTOptions;
// };

template <typename WidgetT> using widget_options_t = typename WidgetOptions<WidgetT>::type;

/**
 * @brief Defines a common widget interface
 */
template <typename WidgetT> class Widget : public crtp_base<Widget<WidgetT>>
{
public:
  WidgetStatus update(Registry& registry, WidgetResources& resources)
  {
    WidgetUpdateCommon(registry, resources);
    return this->derived().UpdateImpl(registry, resources);
  }

  [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(const widget_options_t<WidgetT>& options)
  {
    return WidgetT::CreateImpl(options);
  }
};

}  // namespace tyl::engine
