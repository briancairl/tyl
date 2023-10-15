/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// ImGui
#include <imgui.h>

// Tyl
#include <tyl/async/worker_pool.hpp>
#include <tyl/utility/crtp.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::engine
{

/**
 * @brief Resources used to update a widget
 */
struct WidgetResources
{
  void* gui_context;
  /// Worker pool for deferred work execution
  async::worker_pool worker_pool;
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


/**
 * @brief Defines a common widget interface
 */
template <typename WidgetT> class Widget : public crtp_base<Widget<WidgetT>>
{
public:
  WidgetStatus update(Registry& registry, WidgetResources& resources)
  {
    return this->derived().UpdateImpl(registry, resources);
  }

  template <typename OptionsT> [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(OptionsT&& options)
  {
    return WidgetT::CreateImpl(std::forward<OptionsT>(options));
  }
};

}  // namespace tyl::engine
