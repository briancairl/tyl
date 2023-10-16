/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// EnTT
#include <entt/entt.hpp>

namespace tyl
{

using EntityID = ::entt::entity;
using Registry = ::entt::registry;

template <typename ComponentT> struct Reference
{
  EntityID id;
};

template <typename ComponentT> ComponentT& resolve(Registry& registry, Reference<ComponentT> reference)
{
  return registry.template get<ComponentT>(reference.id);
}

template <typename ComponentT> const ComponentT& resolve(const Registry& registry, Reference<ComponentT> reference)
{
  return registry.template get<ComponentT>(reference.id);
}

}  // namespace tyl
