/**
 * @copyright 2021-present Brian Cairl
 *
 * @file ecs.hpp
 */
#pragma once

// Eigen
#include <entt/entt.hpp>

namespace tyl::ecs
{

using entity = ::entt::entity;

using registry = ::entt::registry;

using namespace ::entt::literals;

template <auto S> using tag = ::entt::tag<S>;

}  // namespace tyl::ecs
