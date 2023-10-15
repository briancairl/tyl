/**
 * @copyright 2023-present Brian Cairl
 *
 * @file entt.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>

// EnTT
#include <entt/fwd.hpp>

namespace tyl
{

template <typename ContextVarT, typename RegistryT> ContextVarT* get(RegistryT&& registry)
{
  return registry.ctx().template find<ContextVarT>();
}

template <typename ContextVarT, typename RegistryT, typename UnaryPredicateT>
ContextVarT* get_if(RegistryT&& registry, UnaryPredicateT pred)
{
  if (auto* const v_ptr = get<ContextVarT>(registry); v_ptr == nullptr or !pred(*v_ptr))
  {
    return nullptr;
  }
  else
  {
    return v_ptr;
  }
}

template <typename ContextVarT, typename RegistryT, typename... Args>
ContextVarT* get_or_emplace(RegistryT&& registry, Args&&... args)
{
  auto& ctx = registry.ctx();
  if (auto* const v_ptr = ctx.template find<ContextVarT>(); v_ptr == nullptr)
  {
    return &ctx.template emplace<ContextVarT>(std::forward<Args>(args)...);
  }
  else
  {
    return v_ptr;
  }
}

}  // namespace tyl
