/**
 * @copyright 2022-present Brian Cairl
 *
 * @file rect.hpp
 */
#pragma once

// EnTT
#include <entt/entt.hpp>

namespace tyl
{

using EntityID = ::entt::entity;
using Registry = ::entt::registry;

template <typename ComponentT> struct Reference
{
  EntityID id;
};

}  // namespace tyl
