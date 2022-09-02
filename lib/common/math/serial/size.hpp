/**
 * @copyright 2022-present Brian Cairl
 *
 * @file size.hpp
 */
#pragma once

// Tyl
#include <tyl/math/size.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>Mat<ScalarT, N, M></code> save implementation
 */
template <typename OArchive, typename ScalarT, int Dims> struct save<OArchive, Size<ScalarT, Dims>>
{
  static_assert(Dims > 0);
  void operator()(OArchive& ar, const Size<ScalarT, Dims>& mat)
  {
    if constexpr (is_trivially_serializable_v<OArchive, Size<ScalarT, Dims>>)
    {
      ar << named{"data", make_packet_fixed_size<Dims>(mat.data())};
    }
    else
    {
      ar << named{"extents", make_sequence(mat.data(), mat.data() + Dims)};
    }
  }
};

/**
 * @brief Archive-generic <code>Mat<ScalarT, N, M></code> load implementation
 */
template <typename IArchive, typename ScalarT, int Dims> struct load<IArchive, Size<ScalarT, Dims>>
{
  static_assert(Dims > 0);
  void operator()(IArchive& ar, Size<ScalarT, Dims>& mat)
  {
    if constexpr (is_trivially_serializable_v<IArchive, Size<ScalarT, Dims>>)
    {
      ar >> named{"data", make_packet_fixed_size<Dims>(mat.data())};
    }
    else
    {
      ar >> named{"extents", make_sequence(mat.data(), mat.data() + Dims)};
    }
  }
};

}  // namespace tyl::serialization
