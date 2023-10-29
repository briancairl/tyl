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
#include <tyl/engine/widget/base.hpp>

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

class PerfMonitor : public WidgetBase<PerfMonitor>
{
  friend class WidgetBase<PerfMonitor>;

public:
  PerfMonitor(PerfMonitor&&) = default;

  ~PerfMonitor();

private:
  static expected<PerfMonitor, WidgetCreationError> CreateImpl(const PerfMonitorOptions& options);

  template <typename StreamT> void SaveImpl(WidgetOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(WidgetIArchive<StreamT>& iar);

  WidgetStatus UpdateImpl(Scene& Scene, WidgetSharedState& shared, const WidgetResources& resources);

  PerfMonitorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  PerfMonitor(const PerfMonitorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
