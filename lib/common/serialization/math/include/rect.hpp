/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// Tyl
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
  void operator()(OArchive& ar, const Rect<ScalarT>& rect) { ar << named{"data", make_packet(&rect)}; }
};

/**
 * @brief Archive-generic <code>Rect<ScalarT></code> load implementation
 */
template <typename IArchive, typename ScalarT> struct load<IArchive, Rect<ScalarT>>
{
  void operator()(IArchive& ar, Rect<ScalarT>& rect) { ar >> named{"data", make_packet(&rect)}; }
};

}  // namespace tyl::serialization
