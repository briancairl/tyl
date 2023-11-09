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

template <typename ArchiveT, typename T, Eigen::Index M, Eigen::Index N>
struct is_trivially_serializable<ArchiveT, Eigen::Matrix<T, M, N>>
    : std::integral_constant<bool, ((M != Eigen::Dynamic) and (M != Eigen::Dynamic))>
{};

template <typename IArchiveT, typename T, Eigen::Index M> struct load<IArchiveT, Eigen::Matrix<T, M, Eigen::Dynamic>>
{
  void operator()(IArchiveT& ar, Eigen::Matrix<T, M, Eigen::Dynamic>& mat)
  {
    Eigen::Index n;
    ar >> named{"cols", n};
    mat.resize(M, n);
    ar >> named{"data", make_packet(mat.data(), mat.size())};
  }
};

template <typename IArchiveT, typename T, Eigen::Index N> struct load<IArchiveT, Eigen::Matrix<T, Eigen::Dynamic, N>>
{
  void operator()(IArchiveT& ar, Eigen::Matrix<T, Eigen::Dynamic, N>& mat)
  {
    Eigen::Index m;
    ar >> named{"rows", m};
    mat.resize(m, N);
    ar >> named{"data", make_packet(mat.data(), mat.size())};
  }
};

template <typename IArchiveT, typename T> struct load<IArchiveT, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
{
  void operator()(IArchiveT& ar, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& mat)
  {
    Eigen::Index m, n;
    ar >> named{"rows", m};
    ar >> named{"cols", n};
    mat.resize(m, n);
    ar >> named{"data", make_packet(mat.data(), mat.size())};
  }
};

template <typename OArchiveT, typename T, Eigen::Index M> struct save<OArchiveT, Eigen::Matrix<T, M, Eigen::Dynamic>>
{
  void operator()(OArchiveT& ar, const Eigen::Matrix<T, M, Eigen::Dynamic>& mat)
  {
    ar << named{"cols", mat.cols()};
    ar << named{"data", make_packet(mat.data(), mat.size())};
  }
};

template <typename OArchiveT, typename T, Eigen::Index N> struct save<OArchiveT, Eigen::Matrix<T, Eigen::Dynamic, N>>
{
  void operator()(OArchiveT& ar, const Eigen::Matrix<T, Eigen::Dynamic, N>& mat)
  {
    ar << named{"rows", mat.rows()};
    ar << named{"data", make_packet(mat.data(), mat.size())};
  }
};

template <typename OArchiveT, typename T> struct save<OArchiveT, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
{
  void operator()(OArchiveT& ar, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& mat)
  {
    ar << named{"rows", mat.rows()};
    ar << named{"cols", mat.cols()};
    ar << named{"data", make_packet(mat.data(), mat.size())};
  }
};

}  // namespace tyl::serialization
