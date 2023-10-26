/*
 * @copyright 2023-present Brian Cairl
 *
 * @file registry.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl
{

template <typename... ComponentTs> struct RegistryComponents
{
  static constexpr bool IsConst = (std::is_const_v<ComponentTs> || ...);
  std::conditional_t<IsConst, const Registry&, Registry&> registry;
};

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

template <typename Archive> struct serialize<Archive, EntityID>
{
  void operator()(Archive& ar, EntityID& entity)
  {
    static_assert(sizeof(EntityID) == sizeof(int));
    auto* entity_as_int = reinterpret_cast<int*>(&entity);
    ar& named{"entity", *entity_as_int};
  }
};


template <typename IArchive, typename... ComponentTs> struct load<IArchive, RegistryComponents<ComponentTs...>>
{
  void operator()(IArchive& iar, RegistryComponents<ComponentTs...>& components)
  {
    SnapshotInputArchive<IArchive> snap_ia{iar, std::addressof(components.registry)};
    entt::snapshot_loader{components.registry}.entities(snap_ia).template component<ComponentTs...>(snap_ia);
  }
};

template <typename OArchive, typename... ComponentTs> struct save<OArchive, RegistryComponents<ComponentTs...>>
{
  void operator()(OArchive& oar, const RegistryComponents<ComponentTs...>& components)
  {
    SnapshotOutputArchive<OArchive> snap_oa{oar, std::addressof(components.registry)};
    entt::snapshot{components.registry}.entities(snap_oa).template component<ComponentTs...>(snap_oa);
  }
};

}  // namespace tyl::serialization
