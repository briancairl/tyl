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
#include <tyl/vec.hpp>

namespace tyl::serialization
{

template <typename ArchiveT, typename T> struct is_trivially_serializable<ArchiveT, Rect<T>> : std::true_type
{};

template <typename ArchiveT, typename T, Eigen::Index M, Eigen::Index N>
struct is_trivially_serializable<ArchiveT, Eigen::Matrix<T, M, N>>
    : std::integral_constant<bool, ((M != Eigen::Dynamic) and (M != Eigen::Dynamic))>
{};

}  // namespace tyl::serialization
