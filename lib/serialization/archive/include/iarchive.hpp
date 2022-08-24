/**
 * @copyright 2022-present Brian Cairl
 *
 * @file iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/label.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

template <typename IArchiveT, typename ValueT> struct load_impl;

template <typename IArchiveT> class iarchive
{
  template <typename ValueT>
  static constexpr bool is_primitive = is_label_v<ValueT> or is_packet_v<ValueT> or is_sequence_v<ValueT>;

public:
  template <typename ValueT> std::enable_if_t<!is_primitive<ValueT>, IArchiveT&> operator>>(ValueT&& payload)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<ValueT>>;
    load_impl<IArchiveT, CleanT>{}(derived(), std::forward<ValueT>(payload));
    return derived();
  }

  template <typename IteratorT> IArchiveT& operator>>(sequence<IteratorT> sequence)
  {
    derived().read_impl(sequence);
    return derived();
  }

  IArchiveT& operator>>(label l)
  {
    derived().read_impl(l);
    return derived();
  }

  template <typename PointerT> IArchiveT& operator>>(basic_packet<PointerT> packet)
  {
    derived().read_impl(packet);
    return derived();
  }

  template <typename PointerT, std::size_t Len> IArchiveT& operator>>(basic_packet_fixed_size<PointerT, Len> packet)
  {
    derived().read_impl(packet);
    return derived();
  }

  iarchive() = default;

private:
  iarchive(const iarchive&) = default;

  constexpr IArchiveT& derived() { return static_cast<IArchiveT&>(*this); }
  constexpr const IArchiveT& derived() const { return static_cast<const IArchiveT&>(*this); }

  static constexpr void read_impl(label _) {}
};

}  // namespace tyl::serialization
