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
#include <tyl/crtp.hpp>
#include <tyl/ecs.hpp>
#include <tyl/engine/internal/widget.hpp>
#include <tyl/expected.hpp>

namespace tyl::engine
{

void WidgetUpdateCommon(Registry& registry, const WidgetResources& resources);

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
  WidgetStatus update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    WidgetUpdateCommon(registry, resources);
    return this->derived().UpdateImpl(registry, shared, resources);
  }

  [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(const widget_options_t<WidgetT>& options)
  {
    return WidgetT::CreateImpl(options);
  }
};

}  // namespace tyl::engine
