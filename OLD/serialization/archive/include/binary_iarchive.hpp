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
#include <tyl/serialization/iarchive.hpp>
#include <tyl/serialization/istream.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

template <typename IStreamT> class binary_iarchive : public iarchive<binary_iarchive<IStreamT>>
{
  using iarchive_base = iarchive<binary_iarchive<IStreamT>>;

  friend iarchive_base;

public:
  explicit binary_iarchive(istream<IStreamT>& is) : is_{static_cast<IStreamT*>(std::addressof(is))} {}

  using iarchive_base::operator>>;
  using iarchive_base::operator&;

private:
  static constexpr void read_impl(label _)
  { /* labels are ignored */
  }

  template <typename IteratorT> constexpr void read_impl(sequence<IteratorT> sequence)
  {
    const auto [first, last] = sequence;
    for (auto itr = first; itr != last; ++itr)
    {
      (*this) >> (*itr);
    }
  }

  template <typename PointerT> constexpr void read_impl(basic_packet<PointerT> packet)
  {
    using value_type = std::remove_pointer_t<PointerT>;
    if constexpr (std::is_void_v<value_type>)
    {
      is_->read(packet.data, packet.len);
    }
    else
    {
      is_->read(packet.data, packet.len * sizeof(value_type));
    }
  }

  template <typename PointerT, std::size_t Len> constexpr void read_impl(basic_packet_fixed_size<PointerT, Len> packet)
  {
    using value_type = std::remove_pointer_t<PointerT>;
    if constexpr (std::is_void_v<value_type>)
    {
      is_->read(packet.data, packet.len);
    }
    else
    {
      is_->read(packet.data, packet.len * sizeof(value_type));
    }
  }

  IStreamT* is_;
};

template <typename IStreamT> binary_iarchive(istream<IStreamT>& is) -> binary_iarchive<IStreamT>;

struct load_trivial
{
  template <typename IStreamT, typename ValueT> void operator()(binary_iarchive<IStreamT>& ar, ValueT& value)
  {
    ar >> make_packet(std::addressof(value));
  }
};

template <typename IStreamT, typename ValueT>
struct load_impl<binary_iarchive<IStreamT>, ValueT>
    : std::conditional_t<
        (is_trivially_serializable_v<binary_iarchive<IStreamT>, ValueT> and
         !load_is_implemented_v<binary_iarchive<IStreamT>, ValueT>),
        load_trivial,
        load<binary_iarchive<IStreamT>, ValueT>>
{};

}  // namespace tyl::serialization
