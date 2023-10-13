/*
 * @copyright 2023-present Brian Cairl
 *
 * @file math.hpp
 */
#pragma once

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl
{

using Registry = ::entt::registry;
using Entity = ::entt::entity;

template <typename... ComponentTs> struct RegistryComponents
{
  Registry* registry;
};

template <typename ComponentT> struct Reference
{
  Entity id;
};

template <typename ComponentT> inline auto& get(Registry& registry, Reference<ComponentT> ref)
{
  return registry.template get<ComponentT>(ref.id);
}

template <typename ComponentT> inline const auto& get(const Registry& registry, Reference<ComponentT> ref)
{
  return registry.template get<ComponentT>(ref.id);
}

}  // namespace tyl

namespace tyl::serialization
{

/**
 * @brief Output archive adaptor for EnTT registry
 *
 *        See https://skypjack.github.io/entt/md_docs_md_entity.html (Archives)
 */
template <typename OArchive> struct SnapshotOutputArchive
{
public:
  SnapshotOutputArchive(OArchive& oa, const Registry* const registry) : oa_{std::addressof(oa)}, registry_{registry} {}

  void operator()(const entt::entity id) { (*oa_) << named{"id", id}; }

  void operator()(const std::underlying_type_t<entt::entity> size) { (*oa_) << named{"size", size}; }

  template <typename T> void operator()(const entt::entity id, const T& value)
  {
    (*oa_) << named{"id", id};
    (*oa_) << named{"value", value};
  }

private:
  OArchive* oa_;
  const Registry* registry_;
};

/**
 * @brief Input archive adaptor for EnTT registry
 *
 *        See https://skypjack.github.io/entt/md_docs_md_entity.html (Archives)
 */
template <typename IArchive> struct SnapshotInputArchive
{
public:
  SnapshotInputArchive(IArchive& ia, Registry* const registry) : ia_{std::addressof(ia)}, registry_{registry} {}

  void operator()(entt::entity& id) { (*ia_) >> named{"id", id}; }

  void operator()(std::underlying_type_t<entt::entity>& size) { (*ia_) >> named{"size", size}; }

  template <typename T> void operator()(entt::entity& id, T& value)
  {
    (*ia_) >> named{"id", id};
    (*ia_) >> named{"value", value};
  }

private:
  IArchive* ia_;
  Registry* registry_;
};

template <typename Archive> struct serialize<Archive, Entity>
{
  void operator()(Archive& ar, Entity& entity)
  {
    static_assert(sizeof(Entity) == sizeof(int));
    auto* entity_as_int = reinterpret_cast<int*>(&entity);
    ar& named{"entity", *entity_as_int};
  }
};


template <typename IArchive, typename... ComponentTs> struct load<IArchive, RegistryComponents<ComponentTs...>>
{
  void operator()(IArchive& ar, RegistryComponents<ComponentTs...>& components)
  {
    components.registry->clear();
    SnapshotInputArchive<IArchive> snap_ia{ar, components.registry};
    entt::snapshot_loader{*components.registry}.entities(snap_ia).template component<ComponentTs...>(snap_ia);
  }
};

template <typename OArchive, typename... ComponentTs> struct save<OArchive, RegistryComponents<ComponentTs...>>
{
  void operator()(OArchive& ar, const RegistryComponents<ComponentTs...>& components)
  {
    SnapshotOutputArchive<OArchive> snap_oa{ar, components.registry};
    entt::snapshot{*components.registry}.entities(snap_oa).template component<ComponentTs...>(snap_oa);
  }
};

}  // namespace tyl::serialization
