/**
 * @copyright 2023-present Brian Cairl
 *
 * @file chrono.hpp
 */
#pragma once

// C++ Standard Library
#include <chrono>

// Tyl
#include <tyl/serialization/archive_fwd.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename RepT, typename PeriodT>
struct save<OArchiveT, std::chrono::duration<RepT, PeriodT>>
{
  void operator()(OArchiveT& oar, const std::chrono::duration<RepT, PeriodT>& duration)
  {
    oar << named{"ticks", duration.count()};
  }
};

template <typename IArchiveT, typename RepT, typename PeriodT>
struct load<IArchiveT, std::chrono::duration<RepT, PeriodT>>
{
  void operator()(IArchiveT& iar, std::chrono::duration<RepT, PeriodT>& path)
  {
    RepT ticks{0};
    iar >> named{"ticks", ticks};
    path = std::chrono::duration<RepT, PeriodT>{ticks};
  }
};

template <typename OStreamT, typename RepT, typename PeriodT>
struct save<binary_oarchive<OStreamT>, std::chrono::duration<RepT, PeriodT>>
{
  void operator()(binary_oarchive<OStreamT>& oar, const std::chrono::duration<RepT, PeriodT>& duration)
  {
    oar << make_packet(std::addressof(duration));
  }
};

template <typename IStreamT, typename RepT, typename PeriodT>
struct load<binary_iarchive<IStreamT>, std::chrono::duration<RepT, PeriodT>>
{
  void operator()(binary_iarchive<IStreamT>& iar, std::chrono::duration<RepT, PeriodT>& duration)
  {
    iar >> make_packet(std::addressof(duration));
  }
};

template <typename OArchiveT, typename ClockT, typename DurationT>
struct save<OArchiveT, std::chrono::time_point<ClockT, DurationT>>
{
  void operator()(OArchiveT& oar, const std::chrono::time_point<ClockT, DurationT>& time_point)
  {
    oar << named{"ticks_since_epoch", time_point.time_since_epoch()};
  }
};

template <typename IArchiveT, typename ClockT, typename DurationT>
struct load<IArchiveT, std::chrono::time_point<ClockT, DurationT>>
{
  void operator()(IArchiveT& iar, std::chrono::time_point<ClockT, DurationT>& time_point)
  {
    DurationT time_since_epoch;
    iar >> named{"time_since_epoch", time_since_epoch};
    time_point = std::chrono::time_point<ClockT, DurationT>{time_since_epoch};
  }
};

template <typename OStreamT, typename ClockT, typename DurationT>
struct save<binary_oarchive<OStreamT>, std::chrono::time_point<ClockT, DurationT>>
{
  void operator()(binary_oarchive<OStreamT>& oar, const std::chrono::time_point<ClockT, DurationT>& time_point)
  {
    oar << make_packet(std::addressof(time_point));
  }
};

template <typename IStreamT, typename ClockT, typename DurationT>
struct load<binary_iarchive<IStreamT>, std::chrono::time_point<ClockT, DurationT>>
{
  void operator()(binary_iarchive<IStreamT>& iar, std::chrono::time_point<ClockT, DurationT>& time_point)
  {
    iar >> make_packet(std::addressof(time_point));
  }
};

}  // namespace tyl::serialization
