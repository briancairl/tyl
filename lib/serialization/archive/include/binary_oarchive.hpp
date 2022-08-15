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

  binary_oarchive& operator<<(binary::const_packet packet)
  {
    os_->write(packet.data, packet.len);
    return *this;
  }

  template <std::size_t Len> binary_oarchive& operator<<(binary::const_packet_fixed_size<Len> packet)
  {
    os_->write(packet.data, packet.len);
    return *this;
  }

private:
  OStreamT* os_;
};

template <typename OStreamT> binary_oarchive(ostream<OStreamT>& os) -> binary_oarchive<OStreamT>;

struct binary_oarchive_trivial_save
{
  template <typename OStreamT, typename ObjectT> void operator()(binary_oarchive<OStreamT>& ar, ObjectT&& target)
  {
    using NoRefObjectT = std::remove_reference_t<ObjectT>;
    ar << binary::const_packet_fixed_size<sizeof(NoRefObjectT)>{reinterpret_cast<const void*>(std::addressof(target))};
  }
};

template <typename OStreamT, typename ObjectT>
struct oarchive_save_impl<binary_oarchive<OStreamT>, ObjectT>
    : std::conditional_t<
        (is_trivially_serializable_v<binary_oarchive<OStreamT>, ObjectT> and
         !save_is_implemented_v<binary_oarchive<OStreamT>, ObjectT>),
        binary_oarchive_trivial_save,
        save<binary_oarchive<OStreamT>, ObjectT>>
{};

}  // namespace tyl::serialization
