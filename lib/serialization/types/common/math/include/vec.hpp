/**
 * @copyright 2022-present Brian Cairl
 *
 * @file vec.hpp
 */
#pragma once

// Tyl
#include <tyl/math/vec.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>Mat<ScalarT, N, M></code> save implementation
 */
template <typename OArchive, typename ScalarT, int N, int M> struct save<OArchive, Mat<ScalarT, N, M>>
{
  static_assert(N > 0);
  static_assert(M > 0);
  void operator()(OArchive& ar, const Mat<ScalarT, N, M>& mat)
  {
    ar << named{"data", make_packet(mat.data(), mat.size())};
  }
};

/**
 * @brief Archive-generic <code>Mat<ScalarT, N, M></code> load implementation
 */
template <typename IArchive, typename ScalarT, int N, int M> struct load<IArchive, Mat<ScalarT, N, M>>
{
  static_assert(N > 0);
  static_assert(M > 0);
  void operator()(IArchive& ar, Mat<ScalarT, N, M>& mat) { ar >> named{"data", make_packet(mat.data(), mat.size())}; }
};

}  // namespace tyl::serialization
