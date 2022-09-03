/**
 * @copyright 2022-present Brian Cairl
 *
 * @file reference.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/ecs/reference.hpp>
#include <tyl/serial/ecs/reader.hpp>
#include <tyl/serialization/named.hpp>

namespace tyl::serialization
{

template <typename ArchiveT, typename ComponentT, typename AccessT> struct save<ArchiveT, ecs::ref<ComponentT, AccessT>>
{
  void operator()(ArchiveT& ar, const ecs::ref<ComponentT, AccessT>& reference)
  {
    ar << named{"guid", reference.guid()};
  }
};

template <typename ComponentT, typename AccessT>
struct enable_registry_access_on_load<ecs::ref<ComponentT, AccessT>> : std::true_type
{};

template <typename ArchiveT, typename ComponentT, typename AccessT>
struct load<ArchiveT, RegistryAccessOnLoad<ecs::ref<ComponentT, AccessT>>>
{
  using stub_type = ecs::to_unresolved_ref_t<ecs::ref<ComponentT, AccessT>>;

  void operator()(ArchiveT& ar, RegistryAccessOnLoad<ecs::ref<ComponentT, AccessT>>& rol)
  {
    ecs::entity reference_guid;
    ar >> named{"guid", reference_guid};

    rol.registry->template emplace<stub_type>(rol.entity, reference_guid);
  }
};

}  // namespace tyl::serialization