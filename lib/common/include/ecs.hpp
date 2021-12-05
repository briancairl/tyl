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

private:
  using handle_type::emplace;
  using handle_type::get_or_emplace;
  using handle_type::remove;
  using handle_type::try_get;
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

template <typename ResourceT> class make_handle_from_this
{
public:
  make_handle_from_this(make_handle_from_this&&) = default;
  make_handle_from_this& operator=(make_handle_from_this&&) = default;

  constexpr auto ref(registry& registry, const entity resource_id) const
  {
    TYL_ASSERT_TRUE(registry.template has<ResourceT>(resource_id));
    return Ref<ResourceT>{registry, resource_id};
  }

  constexpr auto operator()(registry& registry, const entity resource_id) const
  {
    return make_handle_from_this::ref(registry, resource_id);
  }

protected:
  make_handle_from_this() = default;
};

template <typename... ComponentTs> inline auto ref(registry& registry, const entity resource_id)
{
  return Ref<ComponentTs...>{registry, resource_id};
}

}  // namespace tyl::ecs
