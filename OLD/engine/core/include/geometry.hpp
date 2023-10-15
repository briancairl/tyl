/*
 * @copyright 2023-present Brian Cairl
 *
 * @file math.hpp
 */
#pragma once

// Tyl
#include <tyl/math/rect.hpp>
#include <tyl/math/size.hpp>
#include <tyl/math/vec.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

template <typename Archive, typename T, int N> struct serialize<Archive, Size<T, N>>
{
  void operator()(Archive& ar, Size<T, N>& value) { ar& named{"size", make_packet_fixed_size<4>(value.data())}; }
};

template <typename Archive, typename T> struct serialize<Archive, Rect<T>>
{
  void operator()(Archive& ar, Rect<T>& value) { ar& named{"coords", make_packet_fixed_size<4>(value.data())}; }
};

template <typename Archive, typename T, int N> struct serialize<Archive, Vec<T, N>>
{
  void operator()(Archive& ar, Vec<T, N>& value) { ar& named{"values", make_packet_fixed_size<4>(value.data())}; }
};

template <typename Archive, typename T, int N, int M> struct serialize<Archive, Mat<T, N, M>>
{
  void operator()(Archive& ar, Mat<T, N, M>& value)
  {
    ar& named{"values", make_packet_fixed_size<N * M>(value.data())};
  }
};

}  // namespace tyl::serialization
