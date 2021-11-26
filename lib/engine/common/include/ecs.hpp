/**
 * @copyright 2021-present Brian Cairl
 *
 * @file entt.hpp
 */
#pragma once

// Eigen
#include <entt/entt.hpp>

namespace tyl::ecs
{

using Entity = entt::entity;

using Registry = entt::registry;

using namespace entt::literals;

}  // namespace tyl::ecs
