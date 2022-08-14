/**
 * @copyright 2022-present Brian Cairl
 *
 * @file iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

namespace tyl::serialization
{

struct load_not_implemented
{};

template <typename IArchiveT, typename ObjectT> struct load : load_not_implemented
{};

template <typename IArchiveT, typename ObjectT> struct iarchive_load_impl;

template <typename IArchiveT> class iarchive
{
public:
  template <typename PayloadT> IArchiveT& operator>>(PayloadT&& payload)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<PayloadT>>;
    iarchive_load_impl<IArchiveT, CleanT>{}(derived(), std::forward<PayloadT>(payload));
    return derived();
  }

  iarchive() = default;

private:
  iarchive(const iarchive&) = default;

  constexpr IArchiveT& derived() { return static_cast<IArchiveT&>(*this); }
  constexpr const IArchiveT& derived() const { return static_cast<const IArchiveT&>(*this); }
};

}  // namespace tyl::serialization
