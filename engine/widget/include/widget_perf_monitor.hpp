/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_tileset_creator.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/clock.hpp>
#include <tyl/engine/widget.hpp>

namespace tyl::engine
{
class PerfMonitor;

struct PerfMonitorOptions
{
  const char* name = "Performance";
  Clock::Duration sampling_period = Clock::milliseconds(500);
};

template <> struct WidgetOptions<PerfMonitor>
{
  using type = PerfMonitorOptions;
};

class PerfMonitor : public Widget<PerfMonitor>
{
  friend class Widget<PerfMonitor>;

public:
  PerfMonitor(PerfMonitor&&) = default;

  ~PerfMonitor();

private:
  static expected<PerfMonitor, WidgetCreationError> CreateImpl(const PerfMonitorOptions& options);

  template <typename StreamT>
  void SaveImpl(tyl::serialization::binary_oarchive<StreamT>& oar, const Registry& registry) const;

  template <typename StreamT> void LoadImpl(tyl::serialization::binary_iarchive<StreamT>& oar, Registry& registry);

  WidgetStatus UpdateImpl(Registry& registry, WidgetSharedState& shared, const WidgetResources& resources);

  PerfMonitorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  PerfMonitor(const PerfMonitorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
