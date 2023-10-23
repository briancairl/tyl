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
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>

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
  template <typename StreamT>
  void save(tyl::serialization::binary_oarchive<StreamT>& oar, const Registry& registry) const
  {
    this->derived().SaveImpl(oar, registry);
  }

  template <typename StreamT> void load(tyl::serialization::binary_iarchive<StreamT>& iar, Registry& registry)
  {
    this->derived().LoadImpl(iar, registry);
  }

  WidgetStatus update(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources)
  {
    WidgetUpdateCommon(registry, resources);
    return this->derived().UpdateImpl(registry, shared, resources);
  }

  [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(const widget_options_t<WidgetT>& options)
  {
    return WidgetT::CreateImpl(options);
  }

private:
  template <typename StreamT>
  static constexpr void SaveImpl(
    [[maybe_unused]] tyl::serialization::binary_oarchive<StreamT>& oar,
    [[maybe_unused]] const Registry& registry)
  {}

  template <typename StreamT>
  static constexpr void LoadImpl(
    [[maybe_unused]] tyl::serialization::binary_iarchive<StreamT>& iar,
    [[maybe_unused]] const Registry& registry)
  {}
};

}  // namespace tyl::engine
