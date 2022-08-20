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
#include <tyl/serialization/label.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename ValueT> struct save_impl;

template <typename OArchiveT> class oarchive
{
  template <typename ValueT>
  static constexpr bool is_primitive = is_label_v<ValueT> or is_packet_v<ValueT> or is_sequence_v<ValueT>;

public:
  template <typename ValueT> std::enable_if_t<!is_primitive<ValueT>, OArchiveT&> operator<<(const ValueT& value)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<ValueT>>;
    save_impl<OArchiveT, CleanT>{}(derived(), value);
    return derived();
  }

  template <typename IteratorT> OArchiveT& operator<<(const sequence<IteratorT>& sequence)
  {
    derived().write_impl(sequence);
    return derived();
  }

  OArchiveT& operator<<(const label& l)
  {
    derived().write_impl(l);
    return derived();
  }

  template <typename PointerT> OArchiveT& operator<<(const basic_packet<PointerT>& packet)
  {
    derived().write_impl(packet);
    return derived();
  }

  template <typename PointerT, std::size_t Len>
  OArchiveT& operator<<(const basic_packet_fixed_size<PointerT, Len>& packet)
  {
    derived().write_impl(packet);
    return derived();
  }

  oarchive() = default;

private:
  oarchive(const oarchive&) = default;

  constexpr OArchiveT& derived() { return static_cast<OArchiveT&>(*this); }
  constexpr const OArchiveT& derived() const { return static_cast<const OArchiveT&>(*this); }

  static constexpr void write_impl(const label& _) {}
};

}  // namespace tyl::serialization
