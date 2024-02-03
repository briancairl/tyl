/*
 * @copyright 2023-present Brian Cairl
 *
 * @file serialization.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>
#include <type_traits>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::engine
{

template <typename... ComponentTs> struct SerializableRegistry
{
  std::reference_wrapper<Registry> registry;
};

template <typename... ComponentTs> struct ConstSerializableRegistry
{
  std::reference_wrapper<const Registry> registry;
};

template <typename... Listing> struct Components;

template <typename Components> struct MakeSerializableRegistry;

template <typename... Ts> struct MakeSerializableRegistry<Components<Ts...>>
{
  using type = SerializableRegistry<Ts...>;
};

template <typename... Ts> struct MakeSerializableRegistry<const Components<Ts...>>
{
  using type = ConstSerializableRegistry<Ts...>;
};

template <typename Components> using serializable_registry_t = typename MakeSerializableRegistry<Components>::type;

}  // namespace tyl::engine

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

template <typename ArchiveT> struct is_trivially_serializable<ArchiveT, EntityID> : std::true_type
{};

template <typename ArchiveT, typename ComponentT>
struct is_trivially_serializable<ArchiveT, Reference<ComponentT>> : std::true_type
{};

template <typename IArchive, typename... ComponentTs>
struct load<IArchive, engine::SerializableRegistry<ComponentTs...>>
{
  void operator()(IArchive& iar, engine::SerializableRegistry<ComponentTs...> components)
  {
    SnapshotInputArchive<IArchive> snap_ia{iar, std::addressof(components.registry.get())};
    entt::snapshot_loader{components.registry.get()}.entities(snap_ia).template component<ComponentTs...>(snap_ia);
  }
};

template <typename OArchive, typename... ComponentTs>
struct save<OArchive, engine::ConstSerializableRegistry<ComponentTs...>>
{
  void operator()(OArchive& oar, engine::ConstSerializableRegistry<ComponentTs...> components)
  {
    SnapshotOutputArchive<OArchive> snap_oa{oar, std::addressof(components.registry.get())};
    entt::snapshot{components.registry.get()}.entities(snap_oa).template component<ComponentTs...>(snap_oa);
  }
};

}  // namespace tyl::serialization
