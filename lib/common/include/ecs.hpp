/**
 * @copyright 2021-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Eigen
#include <entt/entt.hpp>

namespace tyl::ecs
{

using entity = entt::entity;

using registry = entt::registry;

using namespace entt::literals;

template <typename T> class make_ref_from_this;

template <typename T> class Ref
{
  static_assert(!std::is_reference_v<T>, "Type <T> should not be a reference type");

public:
  using non_const_t = std::remove_const_t<T>;

  constexpr entity id() const { return id_; }

  constexpr operator entity() const { return id(); }

  constexpr non_const_t& value() { return registry_->get<T>(id_); }

  constexpr const non_const_t& value() const { return registry_->get<T>(id_); }

  constexpr operator non_const_t&() { return value(); }

  constexpr operator const non_const_t&() const { return value(); }

  constexpr bool valid() const { return registry_ != nullptr; }

  constexpr operator bool() const { return Ref::valid(); }

  Ref(const Ref& other) = default;

  Ref& operator=(const Ref&) = default;

  Ref(Ref&& other) : registry_{other.registry_}, id_{other.id_} { other.registry_ = nullptr; }

  Ref& operator=(Ref&& other)
  {
    new (this) Ref{std::move(other)};
    return *this;
  }

private:
  Ref(registry& registry, const entity resource_id) : registry_{std::addressof(registry)}, id_{resource_id} {}

  /// Registry that this resource is from
  registry* registry_;

  /// ID of this resource
  entity id_;

  friend class make_ref_from_this<T>;
};

template <typename LhsT, typename RhsT> constexpr bool operator==(const Ref<LhsT>& lhs, const Ref<RhsT>& rhs)
{
  static_assert(std::is_same<std::remove_const_t<LhsT>, std::remove_const_t<RhsT>>());
  return lhs.id() == rhs.id();
}

template <typename LhsT, typename RhsT> constexpr bool operator!=(const Ref<LhsT>& lhs, const Ref<RhsT>& rhs)
{
  static_assert(std::is_same<std::remove_const_t<LhsT>, std::remove_const_t<RhsT>>());
  return lhs.id() != rhs.id();
}

template <typename ResourceT> class make_ref_from_this
{
public:
  make_ref_from_this(make_ref_from_this&&) = default;
  make_ref_from_this& operator=(make_ref_from_this&&) = default;

  constexpr auto ref(registry& registry, const entity resource_id) const
  {
    TYL_ASSERT_TRUE(registry.template has<ResourceT>(resource_id));
    return Ref<ResourceT>{registry, resource_id};
  }

  constexpr auto operator()(registry& registry, const entity resource_id) const
  {
    return make_ref_from_this::ref(registry, resource_id);
  }

protected:
  make_ref_from_this() = default;
};

}  // namespace tyl::ecs
