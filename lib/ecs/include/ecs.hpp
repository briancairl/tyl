/**
 * @copyright 2022-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// EnTT
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

namespace tyl::ecs
{

/// Entity identifier type
using entity = entt::entity;

/// Underying integer type associated with entity
using entity_int_t = std::underlying_type_t<tyl::ecs::entity>;

/// ECS registry type
using registry = entt::registry;

}  // namespace tyl::ecs
