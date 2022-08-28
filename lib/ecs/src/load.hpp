/**
 * @copyright 2022-present Brian Cairl
 *
 * @file load.hpp
 */
#pragma once

// C++ Standard Library
#include <iterator>
#include <type_traits>
#include <vector>

// Tyl
#include <tyl/common/typestr.hpp>
#include <tyl/ecs/ecs.hpp>
#include <tyl/serialization/iarchive.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::serialization
{

template <typename ComponentT> struct enable_registry_access_on_load : std::false_type
{};

template <typename ComponentT>
constexpr bool enable_registry_access_on_load_v = enable_registry_access_on_load<ComponentT>::value;

template <typename ComponentT> struct RegistryAccessOnLoad
{
  ecs::entity entity;
  ecs::registry* const registry;
};

template <typename ComponentT> class DeferredConstruct
{
public:
  template <typename... CTorArgTs> void construct(CTorArgTs&&... ctor_args)
  {
    new (self()) ComponentT{std::forward<CTorArgTs>(ctor_args)...};
  }

  constexpr ComponentT& value() { return *self(); }

  ~DeferredConstruct() { self()->~ComponentT(); }

private:
  ComponentT* self() { return reinterpret_cast<ComponentT*>(buffer_); }

  alignas(ComponentT) std::byte buffer_[sizeof(ComponentT)];
};

/**
 * @brief Wrapper which represents a component and the entity which it belongs to
 */
template <typename ComponentT> struct LoadComponentProxy
{
  constexpr explicit LoadComponentProxy(ecs::registry* const _reg) : reg{_reg} {}

  /// Registry to add component to
  ecs::registry* const reg;
};

/**
 * @brief Dederializes ecs::entity
 */
template <typename ArchiveT> struct load<ArchiveT, ecs::entity>
{
  void operator()(ArchiveT& ar, ecs::entity& e) { ar >> reinterpret_cast<ecs::entity_int_t&>(e); }
};

/**
 * @brief De-serializes LoadComponentProxy
 */
template <typename ArchiveT, typename ComponentT> struct load<ArchiveT, LoadComponentProxy<ComponentT>>
{
  void operator()(ArchiveT& ar, LoadComponentProxy<ComponentT>& proxy)
  {
    ecs::entity e;
    ar >> named{"id", e};
    ar >> named{"value", proxy.reg->template emplace<ComponentT>(e)};
  }
};

/**
 * @brief De-serializes LoadComponentProxy for ComponentT which are not default constructible
 */
template <typename ArchiveT, typename ComponentT>
struct load<ArchiveT, LoadComponentProxy<DeferredConstruct<ComponentT>>>
{
  void operator()(ArchiveT& ar, LoadComponentProxy<DeferredConstruct<ComponentT>>& proxy)
  {
    ecs::entity e;
    ar >> named{"id", reinterpret_cast<ecs::entity_int_t&>(e)};

    DeferredConstruct<ComponentT> deferred;
    ar >> named{"value", deferred};
    proxy.reg->template emplace<ComponentT>(e, std::move(deferred.value()));
  }
};

/**
 * @brief De-serializes LoadComponentProxy for ComponentT which is added to the registry directyly
 */
template <typename ArchiveT, typename ComponentT>
struct load<ArchiveT, LoadComponentProxy<RegistryAccessOnLoad<ComponentT>>>
{
  void operator()(ArchiveT& ar, LoadComponentProxy<RegistryAccessOnLoad<ComponentT>>& proxy)
  {
    ecs::entity e;
    ar >> named{"id", reinterpret_cast<ecs::entity_int_t&>(e)};

    RegistryAccessOnLoad<ComponentT> registry_access{e, proxy.reg};
    ar >> named{"value", registry_access};
  }
};

/**
 * @brief Iterator wrapper used to access a sequence of component values
 */
template <typename ComponentT> class LoadComponentProxyIterator
{
public:
  LoadComponentProxyIterator(std::size_t idx, ecs::registry* const reg) : idx_{idx}, component_proxy_{reg} {}
  LoadComponentProxyIterator& operator++()
  {
    ++idx_;
    return *this;
  }
  constexpr auto& operator*() { return component_proxy_; }
  constexpr bool operator!=(const LoadComponentProxyIterator& other) const { return idx_ != other.idx_; }

private:
  std::size_t idx_;
  LoadComponentProxy<ComponentT> component_proxy_;
};

/**
 * @brief Wrapper which represents an array of components
 */
template <typename ComponentT> struct LoadComponent
{
  ecs::registry* const reg;
  constexpr explicit LoadComponent(ecs::registry* const _reg) : reg{_reg} {}
};

/**
 * @brief Serializes LoadComponent
 */
template <typename ArchiveT> struct load<ArchiveT, LoadComponent<ecs::entity>>
{
  void operator()(ArchiveT& ar, LoadComponent<ecs::entity>& component)
  {
    ecs::entity free_list_start;
    ar >> named{"free", reinterpret_cast<ecs::entity_int_t&>(free_list_start)};

    std::size_t size;
    ar >> named{"size", size};

    std::vector<ecs::entity> v;
    v.resize(size);

    auto* const first = reinterpret_cast<ecs::entity_int_t*>(v.data());
    auto* const last = first + size;
    auto seq = make_sequence(first, last);
    ar >> named{"data", seq};

    component.reg->assign(v.begin(), v.end(), free_list_start);
  }
};

/**
 * @brief Serializes LoadComponent
 */
template <typename ArchiveT, typename ComponentT> struct load<ArchiveT, LoadComponent<ComponentT>>
{
  void operator()(ArchiveT& ar, LoadComponent<ComponentT>& component)
  {
    std::size_t size;
    ar >> named{"size", size};

    auto seq = make_sequence(
      LoadComponentProxyIterator<ComponentT>{0, component.reg}, LoadComponentProxyIterator<ComponentT>{size, nullptr});
    ar >> named{"data", seq};
  }
};

template <typename ArchiveT, typename ComponentT> void load_dispatch(ArchiveT& ar, ecs::registry& reg)
{
  if constexpr (enable_registry_access_on_load_v<ComponentT>)
  {
    LoadComponent<RegistryAccessOnLoad<ComponentT>> lc{std::addressof(reg)};
    ar >> named{typestr<ComponentT>(), lc};
  }
  else if constexpr (std::is_default_constructible_v<ComponentT>)
  {
    LoadComponent<ComponentT> lc{std::addressof(reg)};
    ar >> named{typestr<ComponentT>(), lc};
  }
  else
  {
    LoadComponent<DeferredConstruct<ComponentT>> lc{std::addressof(reg)};
    ar >> named{typestr<ComponentT>(), lc};
  }
}

template <typename... ComponentTs> struct load_components_are_valid : std::true_type
{};

template <typename FirstComponent, typename... OtherComponentTs>
struct load_components_are_valid<FirstComponent, ecs::entity, OtherComponentTs...> : std::false_type
{};

template <typename... OtherComponentTs>
struct load_components_are_valid<ecs::entity, OtherComponentTs...> : load_components_are_valid<OtherComponentTs...>
{};

/**
 * @brief Serializes LoadComponent
 */
template <typename ArchiveT, typename... ComponentTs> struct load<ArchiveT, ecs::reader<ComponentTs...>>
{
  static_assert(sizeof...(ComponentTs), "Must specify at least once component");
  static_assert(
    load_components_are_valid<ComponentTs...>(),
    "ecs::entity must be the first component, or not included in ComponentTs");

  void operator()(ArchiveT& ar, ecs::reader<ComponentTs...> r)
  {
    auto n = ((load_dispatch<ArchiveT, ComponentTs>(ar, *r), 1) + ...);
    (void)n;
  }
};

}  // namespace tyl::serialization


namespace std
{

template <typename ComponentT> struct iterator_traits<::tyl::serialization::LoadComponentProxyIterator<ComponentT>>
{
  using difference_type = std::ptrdiff_t;
  using value_type = ::tyl::serialization::LoadComponentProxy<ComponentT>;
  using pointer = void;
  using reference = value_type&;
  using iterator_category = std::forward_iterator_tag;
};

}  // namespace std