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

  constexpr bool valid() const { return id.has_value(); }

  constexpr operator bool() const { return id.has_value(); }

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

template <typename... ComponentTs>
std::size_t copy(const Registry& from_registry, EntityID from_id, Registry& to_registry, EntityID to_id)
{
  return 0 +
    ((to_registry.template emplace<ComponentTs>(to_id, from_registry.template get<ComponentTs>(from_id)), 1) + ...);
}

}  // namespace tyl
