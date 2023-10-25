/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget_texture_browser.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <memory>
#include <numeric>

// Tyl
#include <tyl/engine/internal/imgui.hpp>
#include <tyl/engine/widget/perf_monitor.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/std/vector.hpp>

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, Clock::Time> : std::true_type
{};

}  // tyl::serialization

using namespace tyl::serialization;

namespace tyl::engine
{
class PerfMonitor::Impl
{
public:
  Impl() { update_time_seconds_.resize(50, 0.f); }

  void Update(const PerfMonitorOptions& options, const WidgetResources& resources)
  {
    if (resources.now > next_sample_time_point_)
    {
      const float update_time = ImGui::GetIO().DeltaTime;
      if (update_time_sample_count_ == 0)
      {
        std::fill(std::begin(update_time_seconds_), std::end(update_time_seconds_), update_time);
        update_time_seconds_avg_ = update_time;
      }
      else
      {
        std::copy(
          std::next(std::begin(update_time_seconds_)),
          std::end(update_time_seconds_),
          std::begin(update_time_seconds_));
        update_time_seconds_.back() = update_time;
        update_time_seconds_avg_ =
          std::accumulate(std::begin(update_time_seconds_), std::end(update_time_seconds_), 0.f);
        update_time_seconds_avg_ /= update_time_seconds_.size();
      }
      next_sample_time_point_ = resources.now + options.sampling_period;
      ++update_time_sample_count_;
    }
    ImGui::PlotLines(
      "##UpdateTime",
      update_time_seconds_.data(),
      update_time_seconds_.size(),
      0.f,
      ImFmt(
        "avg: %f s (%f Hz) [samples %lu]",
        update_time_seconds_avg_,
        1.f / update_time_seconds_avg_,
        update_time_sample_count_),
      0.0f,
      0.01f,
      ImGui::GetContentRegionAvail());
  }

  template <typename OArchive> void Save(OArchive& ar, const Registry& registry) const
  {
    ar << named{"update_time_seconds", update_time_seconds_};
    ar << named{"update_time_sample_count", update_time_sample_count_};
    ar << named{"update_time_seconds_avg", update_time_seconds_avg_};
    ar << named{"next_sample_time_point", next_sample_time_point_};
  }

  template <typename IArchive> void Load(IArchive& ar, Registry& registry)
  {
    ar >> named{"update_time_seconds", update_time_seconds_};
    ar >> named{"update_time_sample_count", update_time_sample_count_};
    ar >> named{"update_time_seconds_avg", update_time_seconds_avg_};
    ar >> named{"next_sample_time_point", next_sample_time_point_};
  }

private:
  std::vector<float> update_time_seconds_;
  std::size_t update_time_sample_count_ = 0;
  float update_time_seconds_avg_;
  Clock::Time next_sample_time_point_ = Clock::Time::min();
};

PerfMonitor::~PerfMonitor() = default;

tyl::expected<PerfMonitor, WidgetCreationError> PerfMonitor::CreateImpl(const PerfMonitorOptions& options)
{
  return PerfMonitor{options, std::make_unique<Impl>()};
}

PerfMonitor::PerfMonitor(const PerfMonitorOptions& options, std::unique_ptr<Impl>&& impl) :
    options_{options}, impl_{std::move(impl)}
{}

template <> void PerfMonitor::SaveImpl(WidgetOArchive<file_handle_ostream>& oar, const Registry& registry) const
{
  impl_->Save(oar, registry);
}

template <> void PerfMonitor::LoadImpl(WidgetIArchive<file_istream>& iar, Registry& registry)
{
  impl_->Load(iar, registry);
}

WidgetStatus PerfMonitor::UpdateImpl(
  [[maybe_unused]] Registry& registry,
  [[maybe_unused]] WidgetSharedState& shared,
  const WidgetResources& resources)
{
  static constexpr auto kStaticWindowFlags = ImGuiWindowFlags_None;
  if (ImGui::Begin(options_.name, nullptr, kStaticWindowFlags))
  {
    impl_->Update(options_, resources);
  }
  ImGui::End();
  return WidgetStatus::kOk;
}

}  // namespace tyl::engine