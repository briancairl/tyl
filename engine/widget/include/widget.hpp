/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget.hpp
 */
#pragma once

// Tyl
#include <tyl/async.hpp>
#include <tyl/crtp.hpp>
#include <tyl/ecs.hpp>
#include <tyl/expected.hpp>

namespace tyl::engine
{

/**
 * @brief Resources used to update a widget
 */
struct WidgetResources
{
  /// Handle to active engine GUI framework context
  void* gui_context;
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

  template <typename OptionsT> [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(OptionsT&& options)
  {
    return WidgetT::CreateImpl(std::forward<OptionsT>(options));
  }
};

}  // namespace tyl::engine
