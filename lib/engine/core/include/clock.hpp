/**
 * @copyright 2023-present Brian Cairl
 *
 * @file clock.hpp
 */
#pragma once

// C++ Standard Library
#include <chrono>
#include <optional>

namespace tyl::engine::core
{

template <typename ClockT> struct ClockInterface
{
  using StdClock = ClockT;
  using Time = typename ClockT::time_point;
  using Duration = typename ClockT::duration;

  static constexpr Time now() { return ClockT::now(); }

  template <typename SecondsT> static constexpr Duration seconds(SecondsT&& s)
  {
    return std::chrono::seconds(std::forward<SecondsT>(s));
  }

  template <typename MilliSecondsT> static constexpr Duration millis(MilliSecondsT&& ms)
  {
    return std::chrono::milliseconds(std::forward<MilliSecondsT>(ms));
  }

  template <typename MicroSecondsT> static constexpr Duration micros(MicroSecondsT&& us)
  {
    return std::chrono::microseconds(std::forward<MicroSecondsT>(us));
  }

  template <typename NanoSecondsT> static constexpr Duration nanos(NanoSecondsT&& ns)
  {
    return std::chrono::nanoseconds(std::forward<NanoSecondsT>(ns));
  }
};

using Clock = ClockInterface<std::chrono::steady_clock>;

using SystemClock = ClockInterface<std::chrono::system_clock>;

template <typename T, typename ClockInterfaceT = Clock> struct Stamped
{
public:
  using StampType = typename ClockInterfaceT::Time;

  const StampType& stamp() const { return stamp_; }

  void reset()
  {
    stamp_ = StampType::min();
    value_.reset();
  }

  bool is_valid() const { return value_.has_value(); }

  const T& get() const { return *value_; }

  template <typename... ArgTs> const T& emplace(const StampType& stamp, ArgTs&&... args)
  {
    stamp_ = stamp;
    value_.emplace(std::forward<ArgTs>(args)...);
    return *value_;
  }

  constexpr T& operator*() { return value_.operator*(); }
  constexpr const T& operator*() const { return value_.operator*(); }

  constexpr T* operator->() { return value_.operator->(); }
  constexpr const T* operator->() const { return value_.operator->(); }

private:
  StampType stamp_;
  std::optional<T> value_;
};

}  // namespace tyl::engine::core
