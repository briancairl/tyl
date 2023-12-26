/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tags.hpp
 */
#pragma once

// Tyl
#include <tyl/serialization/object.hpp>

namespace tyl::engine::tags
{

struct Hidden
{};

struct DebugInfoVisible
{};

}  // namespace tyl::engine::tags

namespace tyl::serialization
{

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::tags::Hidden> : std::true_type
{};

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, engine::tags::DebugInfoVisible> : std::true_type
{};

}  // namespace tyl::serialization
