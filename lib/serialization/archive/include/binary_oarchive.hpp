/**
 * @copyright 2022-present Brian Cairl
 *
 * @file binary_oarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/oarchive.hpp>
#include <tyl/serialization/ostream.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

template <typename OStreamT> class binary_oarchive : public oarchive<binary_oarchive<OStreamT>>
{
  using oarchive_base = oarchive<binary_oarchive<OStreamT>>;

  friend oarchive_base;

public:
  explicit binary_oarchive(ostream<OStreamT>& os) : os_{static_cast<OStreamT*>(std::addressof(os))} {}

  using oarchive_base::operator<<;

private:
  static constexpr void write_impl(const label& _)
  { /* labels are ignored */
  }

  template <typename IteratorT> constexpr void write_impl(const sequence<IteratorT>& sequence)
  {
    const auto [first, last] = sequence;
    for (auto itr = first; itr != last; ++itr)
    {
      (*this) << (*itr);
    }
  }

  template <typename PointerT> constexpr void write_impl(const basic_packet<PointerT>& packet)
  {
    using value_type = std::remove_pointer_t<PointerT>;
    if constexpr (std::is_void_v<value_type>)
    {
      os_->write(packet.data, packet.len);
    }
    else
    {
      os_->write(packet.data, packet.len * sizeof(value_type));
    }
  }

  template <typename PointerT, std::size_t Len>
  constexpr void write_impl(const basic_packet_fixed_size<PointerT, Len>& packet)
  {
    using value_type = std::remove_pointer_t<PointerT>;
    if constexpr (std::is_void_v<value_type>)
    {
      os_->write(packet.data, packet.len);
    }
    else
    {
      os_->write(packet.data, packet.len * sizeof(value_type));
    }
  }

  OStreamT* os_;
};

template <typename OStreamT> binary_oarchive(ostream<OStreamT>& os) -> binary_oarchive<OStreamT>;

struct save_trivial
{
  template <typename OStreamT, typename ValueT> void operator()(binary_oarchive<OStreamT>& ar, const ValueT& value)
  {
    ar << make_packet(std::addressof(value));
  }
};

template <typename OStreamT, typename ValueT>
struct save_impl<binary_oarchive<OStreamT>, ValueT>
    : std::conditional_t<
        (is_trivially_serializable_v<binary_oarchive<OStreamT>, ValueT> and
         !save_is_implemented_v<binary_oarchive<OStreamT>, ValueT>),
        save_trivial,
        save<binary_oarchive<OStreamT>, ValueT>>
{};

}  // namespace tyl::serialization
