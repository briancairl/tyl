/**
 * @copyright 2021 Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// EnTT
#include <entt/entt.hpp>

// Boost
#include <boost/serialization/nvp.hpp>

namespace boost::serialization
{

template <typename ArchiveT> inline void serialize(ArchiveT& ar, entt::entity& id, const unsigned int version)
{
  using int_type = std::underlying_type_t<entt::entity>;
  ar & make_nvp("id", *reinterpret_cast<int_type*>(&id));
}

}  // namespace boost::serialization

namespace tyl::ecs
{

template<typename ComponentT>
struct BeforeComponentLoad
{
  template<typename ArchiveT>
  constexpr void operator()(ArchiveT& ar, entt::registry& registry, ComponentT& value, const entt::entity id) {}
};

template<typename ComponentT>
struct AfterComponentLoad
{
  template<typename ArchiveT>
  constexpr void operator()(ArchiveT& ar, entt::registry& registry, ComponentT& value, const entt::entity id) {}
};

template<typename ComponentT>
struct BeforeComponentSave
{
  template<typename ArchiveT>
  constexpr void operator()(ArchiveT& ar, entt::registry& registry, const ComponentT& value, const entt::entity id) {}
};

template<typename ComponentT>
struct AfterComponentSave
{
  template<typename ArchiveT>
  constexpr void operator()(ArchiveT& ar, entt::registry& registry, const ComponentT& value, const entt::entity id) {}
};

}  // namespace tyl::ecs