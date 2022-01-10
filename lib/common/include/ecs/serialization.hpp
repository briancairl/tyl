/**
 * @copyright 2021-present Brian Cairl
 *
 * @file entity.inl
 */
#pragma once

// Tyl
#include <tyl/ecs/typedef.hpp>

namespace tyl::ecs
{

template <typename... ComponentTs, typename ArchiveT> void save(ArchiveT& ar, const registry& reg);

template <typename... ComponentTs, typename ArchiveT> void load(ArchiveT& ar, registry& reg);

}  // namespace tyl::ecs

// Tyl
#include <tyl/ecs/serialization/entity.inl>
#include <tyl/ecs/serialization/registry.inl>
#include <tyl/ecs/serialization/tag.inl>
