/**
 * @copyright 2022-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// EnTT
#include <entt/entt.hpp>

namespace tyl
{

using EntityID = ::entt::entity;
using Registry = ::entt::registry;

template <typename ComponentT> struct Reference
{
  std::optional<EntityID> id;
  constexpr void reset() { this->id.reset(); }
  constexpr Reference<ComponentT>& operator=(const EntityID id)
  {
    this->id.emplace(id);
    return *this;
  }
};

template <typename ComponentT> bool operator==(std::nullptr_t _, const Reference<ComponentT>& ref)
{
  return !ref.id.has_value();
};

template <typename ComponentT> bool operator==(const Reference<ComponentT>& ref, std::nullptr_t _)
{
  return !ref.id.has_value();
};

template <typename ComponentT> bool operator!=(std::nullptr_t _, const Reference<ComponentT>& ref)
{
  return ref.id.has_value();
};

template <typename ComponentT> bool operator!=(const Reference<ComponentT>& ref, std::nullptr_t _)
{
  return ref.id.has_value();
};

template <typename ComponentT> bool is_valid(Registry& registry, const Reference<ComponentT>& reference)
{
  return (reference != nullptr) and registry.template any_of<ComponentT>(*reference.id);
}

template <typename ComponentT> ComponentT& resolve(Registry& registry, const Reference<ComponentT>& reference)
{
  return registry.template get<ComponentT>(*reference.id);
}

template <typename ComponentT>
const ComponentT& resolve(const Registry& registry, const Reference<ComponentT>& reference)
{
  return registry.template get<ComponentT>(*reference.id);
}

template <typename ComponentT> ComponentT* maybe_resolve(Registry& registry, const Reference<ComponentT>& reference)
{
  return is_valid(registry, reference) ? std::addressof(registry.template get<ComponentT>(*reference.id)) : nullptr;
}

template <typename ComponentT>
const ComponentT* maybe_resolve(const Registry& registry, const Reference<ComponentT>& reference)
{
  return is_valid(registry, reference) ? std::addressof(registry.template get<ComponentT>(*reference.id)) : nullptr;
}

}  // namespace tyl
