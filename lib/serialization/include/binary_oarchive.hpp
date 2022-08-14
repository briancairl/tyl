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
#include <tyl/serialization/binary/payload.hpp>
#include <tyl/serialization/oarchive.hpp>
#include <tyl/serialization/ostream.hpp>

namespace tyl::serialization
{

template <typename OStreamT> class binary_oarchive : public oarchive<binary_oarchive<OStreamT>>
{
  using oarchive_base = oarchive<binary_oarchive<OStreamT>>;

  friend oarchive_base;

public:
  explicit binary_oarchive(ostream<OStreamT>& os) : os_{static_cast<OStreamT*>(std::addressof(os))} {}

  using oarchive_base::operator<<;

  binary_oarchive& operator<<(binary::const_payload payload)
  {
    os_->write(payload.data, payload.len);
    return *this;
  }

  template <std::size_t Len> binary_oarchive& operator<<(binary::const_payload_fixed_size<Len> payload)
  {
    os_->write(payload.data, payload.len);
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
    ar << binary::const_payload_fixed_size<sizeof(NoRefObjectT)>{reinterpret_cast<const void*>(std::addressof(target))};
  }
};

template <typename OStreamT, typename ObjectT>
struct oarchive_save_impl<binary_oarchive<OStreamT>, ObjectT>
    : std::conditional_t<
        (std::is_trivial_v<ObjectT> and
         std::is_base_of_v<save_not_implemented, save<binary_oarchive<OStreamT>, ObjectT>>),
        binary_oarchive_trivial_save,
        save<binary_oarchive<OStreamT>, ObjectT>>
{};

}  // namespace tyl::serialization
