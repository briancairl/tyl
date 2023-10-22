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

template <typename OtherComponentT, typename ComponentT>
Reference<OtherComponentT> adopt(Reference<ComponentT> reference)
{
  return Reference<OtherComponentT>{reference.id};
}

template <typename ComponentT> ComponentT& resolve(Registry& registry, Reference<ComponentT> reference)
{
  return registry.template get<ComponentT>(reference.id);
}

template <typename ComponentT> const ComponentT& resolve(const Registry& registry, Reference<ComponentT> reference)
{
  return registry.template get<ComponentT>(reference.id);
}

template <typename ComponentT> ComponentT* maybe_resolve(Registry& registry, Reference<ComponentT> reference)
{
  return registry.template any_of<ComponentT>(reference.id)
    ? std::addressof(registry.template get<ComponentT>(reference.id))
    : nullptr;
}

template <typename ComponentT>
const ComponentT* maybe_resolve(const Registry& registry, Reference<ComponentT> reference)
{
  return registry.template any_of<ComponentT>(reference.id)
    ? std::addressof(registry.template get<ComponentT>(reference.id))
    : nullptr;
}

}  // namespace tyl
