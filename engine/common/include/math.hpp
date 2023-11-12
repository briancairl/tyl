/**
 * @copyright 2023-present Brian Cairl
 *
 * @file math.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/rect.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>
#include <tyl/vec.hpp>

namespace tyl::serialization
{

template <typename ArchiveT, typename T> struct is_trivially_serializable<ArchiveT, Rect<T>> : std::true_type
{};


template <typename IArchiveT, typename T, int M, int N, int Options>
struct load<IArchiveT, Eigen::Matrix<T, M, N, Options>>
{
  void operator()(IArchiveT& ar, Eigen::Matrix<T, M, N, Options>& mat)
  {
    if constexpr (N == Eigen::Dynamic || M == Eigen::Dynamic)
    {
      Eigen::Index rows, cols;
      ar >> named{"rows", rows};
      ar >> named{"cols", cols};
      mat.resize(rows, cols);
    }
    ar >> named{"data", make_packet(mat.data(), mat.size())};
  }
};


template <typename OArchiveT, typename T, int M, int N, int Options>
struct save<OArchiveT, Eigen::Matrix<T, M, N, Options>>
{
  void operator()(OArchiveT& ar, const Eigen::Matrix<T, M, N, Options>& mat)
  {
    if constexpr (N == Eigen::Dynamic || M == Eigen::Dynamic)
    {
      ar << named{"rows", mat.rows()};
      ar << named{"cols", mat.cols()};
    }
    ar << named{"data", make_packet(mat.data(), mat.size())};
  }
};

}  // namespace tyl::serialization
