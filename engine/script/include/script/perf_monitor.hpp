/**
 * @copyright 2023-present Brian Cairl
 *
 * @file perf_monitor.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/clock.hpp>
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class PerfMonitor;

struct PerfMonitorOptions
{
  const char* name = "Performance";
  Clock::Duration sampling_period = Clock::milliseconds(500);
};

template <> struct ScriptOptions<PerfMonitor>
{
  using type = PerfMonitorOptions;
};

class PerfMonitor : public ScriptBase<PerfMonitor>
{
  friend class ScriptBase<PerfMonitor>;

public:
  PerfMonitor(PerfMonitor&&) = default;

  ~PerfMonitor();

private:
  static expected<PerfMonitor, ScriptCreationError> CreateImpl(const PerfMonitorOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& Scene, ScriptSharedState& shared, const ScriptResources& resources);

  PerfMonitorOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  PerfMonitor(const PerfMonitorOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
