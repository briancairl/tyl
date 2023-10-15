/**
 * @copyright 2023-present Brian Cairl
 *
 * @file clock.hpp
 */
#pragma once

// C++ Standard Library
#include <chrono>

namespace tyl
{

template <typename ClockT> struct ClockInterface
{
  using Time = typename ClockT::time_point;
  using Duration = typename ClockT::duration;

  static constexpr Time now() { return ClockT::now(); }

  template <typename SecondsT> static constexpr Duration seconds(SecondsT&& s)
  {
    return std::chrono::seconds{std::forward<SecondsT>(s)};
  }

  template <typename MilliSecondsT> static constexpr Duration milliseconds(MilliSecondsT&& ms)
  {
    return std::chrono::milliseconds{std::forward<MilliSecondsT>(ms)};
  }

  template <typename MicroSecondsT> static constexpr Duration microseconds(MicroSecondsT&& us)
  {
    return std::chrono::microseconds{std::forward<MicroSecondsT>(us)};
  }

  template <typename NanoSecondsT> static constexpr Duration nanoseconds(NanoSecondsT&& ns)
  {
    return std::chrono::nanoseconds{std::forward<NanoSecondsT>(ns)};
  }
};

using Clock = ClockInterface<std::chrono::steady_clock>;
using SystemClock = ClockInterface<std::chrono::system_clock>;

}  // namespace tyl
