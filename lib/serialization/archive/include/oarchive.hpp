/**
 * @copyright 2022-present Brian Cairl
 *
 * @file oarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename ObjectT> struct oarchive_save_impl;

template <typename OArchiveT> class oarchive
{
public:
  template <typename PayloadT> OArchiveT& operator<<(PayloadT&& payload)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<PayloadT>>;
    oarchive_save_impl<OArchiveT, CleanT>{}(derived(), std::forward<PayloadT>(payload));
    return derived();
  }

  oarchive() = default;

private:
  oarchive(const oarchive&) = default;

  constexpr OArchiveT& derived() { return static_cast<OArchiveT&>(*this); }
  constexpr const OArchiveT& derived() const { return static_cast<const OArchiveT&>(*this); }
};

}  // namespace tyl::serialization
