/**
 * @copyright 2021-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// Eigen
#include <entt/entt.hpp>

namespace tyl::ecs
{

using entity = entt::entity;

using registry = entt::registry;

using namespace entt::literals;

/**
 * @brief Wrapper around \c entt::handle with component type guarantees
 */
template <typename... ComponentTs> struct Ref : public entt::handle
{
public:
  using handle_type = entt::handle;
  using handle_type::handle_type;

  constexpr decltype(auto) operator*() { return this->template get<ComponentTs...>(); }

  constexpr decltype(auto) operator*() const { return this->template get<ComponentTs...>(); }
};

template <typename... ComponentTs>
constexpr bool operator<(const Ref<ComponentTs...>& lhs, const Ref<ComponentTs...>& rhs)
{
  return lhs.entity() < rhs.entity();
}

template <typename... ComponentTs>
constexpr bool operator>(const Ref<ComponentTs...>& lhs, const Ref<ComponentTs...>& rhs)
{
  return lhs.entity() > rhs.entity();
}

template <typename... ComponentTs>
constexpr bool operator==(const Ref<ComponentTs...>& lhs, const Ref<ComponentTs...>& rhs)
{
  return lhs.entity() == rhs.entity();
}

template <typename... ComponentTs>
constexpr bool operator!=(const Ref<ComponentTs...>& lhs, const Ref<ComponentTs...>& rhs)
{
  return lhs.entity() != rhs.entity();
}

template <typename T> struct is_ref : std::false_type
{};

template <typename T> struct is_ref<Ref<T>> : std::true_type
{};

template <typename... ComponentTs> inline auto ref(registry& registry, const entity resource_id)
{
  return Ref<ComponentTs...>{registry, resource_id};
}

template <typename ComponentT, typename... ParentComponentTs> inline auto ref(const Ref<ParentComponentTs...> parent)
{
  // TODO(enhancment) do some sort of compile-time consistency checking between (ComponentTs) and (ParentComponentTs)
  if constexpr (is_ref<ComponentT>())
  {
    return parent.registry()->template get<ComponentT>(parent.entity());
  }
  else
  {
    return Ref<ComponentT>{*parent.registry(), parent.entity()};
  }
}

}  // namespace tyl::ecs
