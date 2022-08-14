/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/binary/payload.hpp>
#include <tyl/serialization/iarchive.hpp>
#include <tyl/serialization/istream.hpp>

namespace tyl::serialization
{

template <typename IStreamT> class binary_iarchive : public iarchive<binary_iarchive<IStreamT>>
{
  using iarchive_base = iarchive<binary_iarchive<IStreamT>>;

  friend iarchive_base;

public:
  explicit binary_iarchive(istream<IStreamT>& is) : is_{static_cast<IStreamT*>(std::addressof(is))} {}

  using iarchive_base::operator>>;

  binary_iarchive& operator>>(binary::payload packet)
  {
    is_->read(packet.data, packet.len);
    return *this;
  }

  template <std::size_t Len> binary_iarchive& operator>>(binary::payload_fixed_size<Len> packet)
  {
    is_->read(packet.data, packet.len);
    return *this;
  }

private:
  IStreamT* is_;
};

template <typename IStreamT> binary_iarchive(istream<IStreamT>& is) -> binary_iarchive<IStreamT>;

struct binary_iarchive_trivial_load
{
  template <typename IStreamT, typename ObjectT> void operator()(binary_iarchive<IStreamT>& ar, ObjectT&& target)
  {
    using NoRefObjectT = std::remove_reference_t<ObjectT>;
    ar >> binary::payload_fixed_size<sizeof(NoRefObjectT)>{reinterpret_cast<void*>(std::addressof(target))};
  }
};

template <typename IStreamT, typename ObjectT>
struct iarchive_load_impl<binary_iarchive<IStreamT>, ObjectT>
    : std::conditional_t<
        (std::is_trivial_v<ObjectT> and
         std::is_base_of_v<load_not_implemented, load<binary_iarchive<IStreamT>, ObjectT>>),
        binary_iarchive_trivial_load,
        load<binary_iarchive<IStreamT>, ObjectT>>
{};

}  // namespace tyl::serialization
