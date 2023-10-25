/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <string_view>

// Tyl
#include <tyl/crtp.hpp>
#include <tyl/ecs.hpp>
#include <tyl/engine/internal/widget.hpp>
#include <tyl/expected.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::engine
{

template <typename WidgetT> struct WidgetOptions;
// {
//   using type = WidgetTOptions;
// };

template <typename WidgetT> using widget_options_t = typename WidgetOptions<WidgetT>::type;

/**
 * @brief Defines a common widget interface
 */
template <typename WidgetT> class WidgetBase : public crtp_base<WidgetBase<WidgetT>>
{
public:
  template <typename StreamT> void save(WidgetOArchive<StreamT>& oar, const Registry& registry) const
  {
    this->derived().SaveImpl(oar, registry);
  }

  template <typename StreamT> void load(WidgetIArchive<StreamT>& iar, Registry& registry)
  {
    this->derived().LoadImpl(iar, registry);
  }

  WidgetStatus update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    return this->derived().UpdateImpl(registry, shared, resources);
  }

  [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(const widget_options_t<WidgetT>& options)
  {
    return WidgetT::CreateImpl(options);
  }

  [[nodiscard]] static std::string_view name() { return WidgetT::NameImpl(); }

private:
  template <typename StreamT>
  static constexpr void
  SaveImpl([[maybe_unused]] WidgetOArchive<StreamT>& oar, [[maybe_unused]] const Registry& registry)
  {}

  template <typename StreamT>
  static constexpr void
  LoadImpl([[maybe_unused]] WidgetIArchive<StreamT>& iar, [[maybe_unused]] const Registry& registry)
  {}
};

}  // namespace tyl::engine
