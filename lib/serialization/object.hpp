/**
 * @copyright 2022-present Brian Cairl
 *
 * @file object.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

namespace tyl::serialization
{

struct load_not_implemented
{};

template <typename IArchiveT, typename ObjectT> struct load : load_not_implemented
{};

template <typename IArchiveT, typename ObjectT>
struct load_is_implemented
    : std::integral_constant<bool, !std::is_base_of_v<load_not_implemented, load<IArchiveT, ObjectT>>>
{};

template <typename IArchiveT, typename ObjectT>
constexpr bool load_is_implemented_v = load_is_implemented<IArchiveT, ObjectT>::value;

struct save_not_implemented
{};

template <typename OArchiveT, typename ObjectT> struct save : save_not_implemented
{};

template <typename OArchiveT, typename ObjectT>
struct save_is_implemented
    : std::integral_constant<bool, !std::is_base_of_v<save_not_implemented, save<OArchiveT, ObjectT>>>
{};

template <typename OArchiveT, typename ObjectT>
constexpr bool save_is_implemented_v = save_is_implemented<OArchiveT, ObjectT>::value;

template <typename ArchiveT, typename ObjectT> struct is_trivially_serializable : std::is_trivial<ObjectT>
{};

template <typename ArchiveT, typename ObjectT>
const bool is_trivially_serializable_v = is_trivially_serializable<ArchiveT, ObjectT>::value;

}  // namespace tyl::serialization
