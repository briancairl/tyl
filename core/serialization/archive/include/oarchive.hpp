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
#include <tyl/crtp.hpp>
#include <tyl/serialization/label.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

template <typename OArchiveT, typename ValueT> struct save_impl;

template <typename OArchiveT> class oarchive : public crtp_base<oarchive<OArchiveT>>
{
  template <typename ValueT>
  static constexpr bool is_primitive = is_label_v<ValueT> or is_packet_v<ValueT> or is_sequence_v<ValueT>;

public:
  template <typename ValueT> OArchiveT& operator&(const ValueT& value)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<ValueT>>;
    save_impl<OArchiveT, CleanT>{}(this->derived(), value);
    return this->derived();
  }

  template <typename ValueT> std::enable_if_t<!is_primitive<ValueT>, OArchiveT&> operator<<(const ValueT& value)
  {
    using CleanT = std::remove_const_t<std::remove_reference_t<ValueT>>;
    save_impl<OArchiveT, CleanT>{}(this->derived(), value);
    return this->derived();
  }

  template <typename IteratorT> OArchiveT& operator<<(const sequence<IteratorT>& sequence)
  {
    this->derived().write_impl(sequence);
    return this->derived();
  }

  OArchiveT& operator<<(const label& l)
  {
    this->derived().write_impl(l);
    return this->derived();
  }

  template <typename PointerT> OArchiveT& operator<<(const basic_packet<PointerT>& packet)
  {
    this->derived().write_impl(packet);
    return this->derived();
  }

  template <typename PointerT, std::size_t Len>
  OArchiveT& operator<<(const basic_packet_fixed_size<PointerT, Len>& packet)
  {
    this->derived().write_impl(packet);
    return this->derived();
  }

  oarchive() = default;

private:
  oarchive(const oarchive&) = default;

  static constexpr void write_impl(const label& _) {}
};

}  // namespace tyl::serialization
