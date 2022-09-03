/**
 * @copyright 2022-present Brian Cairl
 *
 * @file reference.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>

// Tyl
#include <tyl/ecs/ecs.hpp>

namespace tyl::ecs
{

/**
 * @brief Represents an indirect reference to a held resource
 */
template <typename ComponentT, typename AccessT = registry> class ref
{
public:
  static_assert(!std::is_void_v<AccessT>, "AccessT may not be a void type");
  static_assert(!std::is_pointer_v<AccessT>, "AccessT may not be a raw pointer type");

  ref(entity id, const AccessT& parent) : guid_{id}, value_{parent} {}

  constexpr entity guid() const { return guid_; }

  constexpr AccessT& value() { return value_; }
  constexpr const AccessT& value() const { return value_; }

  constexpr AccessT& operator*() { return ref::value(); }
  constexpr const AccessT& operator*() const { return ref::value(); }

  constexpr AccessT* operator->() { return &value_; }
  constexpr const AccessT* operator->() const { return &value_; }

private:
  entity guid_;
  AccessT value_;
};

/**
 * @brief Represents an indirect reference to a held resource
 */
template <typename ComponentT> class ref<ComponentT, registry>
{
public:
  ref(entity id, registry& reg) : guid_{id}, reg_{&reg} {}

  constexpr entity guid() const { return guid_; }

  constexpr ComponentT& value() { return reg_->template get<ComponentT>(guid_); }
  constexpr const ComponentT& value() const { return reg_->template get<ComponentT>(guid_); }

  constexpr ComponentT& operator*() { return ref::value(); }
  constexpr const ComponentT& operator*() const { return ref::value(); }

  constexpr ComponentT* operator->() { return &ref::value(); }
  constexpr const ComponentT* operator->() const { return &ref::value(); }

private:
  entity guid_;
  registry* const reg_;
};

template <typename RefT> struct is_ref : std::false_type
{};

template <typename ComponentT, typename AccessT> struct is_ref<ref<ComponentT, AccessT>> : std::true_type
{};

template <typename RefT> constexpr bool is_ref_v = is_ref<RefT>::value;

template <typename ComponentT, typename AccessT> struct unresolved_ref
{
  entity guid;
};

template <typename RefT> struct to_unresolved_ref;

template <typename ComponentT, typename AccessT> struct to_unresolved_ref<ref<ComponentT, AccessT>>
{
  using type = unresolved_ref<ComponentT, AccessT>;
};

template <typename RefT> using to_unresolved_ref_t = typename to_unresolved_ref<RefT>::type;

template <typename RefT> struct follow_reference;

template <typename ComponentT, typename AccessT> struct follow_reference<ref<ComponentT, AccessT>>
{
  using ref_type = ref<ComponentT, AccessT>;
  using stub_type = to_unresolved_ref_t<ref_type>;

  static void fulfill(registry& reg, const entity e, const stub_type stub)
  {
    reg.template emplace<ref_type>(e, stub.guid, reg.template get<ComponentT>(stub.guid));
    reg.template remove<stub_type>(e);
  }
};

template <typename ComponentT> struct follow_reference<ref<ComponentT, registry>>
{
  using ref_type = ref<ComponentT, registry>;
  using stub_type = to_unresolved_ref_t<ref_type>;

  static void fulfill(registry& reg, const entity e, const stub_type stub)
  {
    reg.template emplace<ref_type>(e, stub.guid, reg);
    reg.template remove<stub_type>(e);
  }
};

template <typename RefT> void resolve_reference(registry& reg)
{
  static_assert(is_ref_v<RefT>, "RefT must be a reference type");

  using stub_type = to_unresolved_ref_t<RefT>;

  reg.template view<stub_type>().each(
    [&reg](entity e, const stub_type uref) { follow_reference<RefT>::fulfill(reg, e, uref); });
}

template <typename... RefT> void resolve_references(registry& reg)
{
  const int _ = ((resolve_reference<RefT>(reg), 1) + ...);
  (void)_;
}

}  // namespace tyl::ecs
