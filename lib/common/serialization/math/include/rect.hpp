/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// Tyl
#include <tyl/common/serialization/math/vec.hpp>
#include <tyl/math/rect.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>Rect<ScalarT></code> save implementation
 */
template <typename OArchive, typename ScalarT> struct save<OArchive, Rect<ScalarT>>
{
  void operator()(OArchive& ar, const Rect<ScalarT>& rect)
  {
    if constexpr (is_trivially_serializable_v<OArchive, Rect<ScalarT>>)
    {
      ar << named{"data", make_packet(&rect)};
    }
    else
    {
      ar << named{"min", Vec<ScalarT, 2>{rect.min()}};
      ar << named{"max", Vec<ScalarT, 2>{rect.max()}};
    }
  }
};

/**
 * @brief Archive-generic <code>Rect<ScalarT></code> load implementation
 */
template <typename IArchive, typename ScalarT> struct load<IArchive, Rect<ScalarT>>
{
  void operator()(IArchive& ar, Rect<ScalarT>& rect)
  {
    if constexpr (is_trivially_serializable_v<IArchive, Rect<ScalarT>>)
    {
      ar >> named{"data", make_packet(&rect)};
    }
    else
    {
      Vec<ScalarT, 2> min_corner, max_corner;
      ar >> named{"min", min_corner};
      ar >> named{"max", max_corner};
      rect = Rect<ScalarT>{min_corner, max_corner};
    }
  }
};

}  // namespace tyl::serialization
