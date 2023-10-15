/**
 * @copyright 2022-present Brian Cairl
 *
 * @file includes.hpp
 */
#pragma once

namespace tyl::meta
{

/**
 * @brief Checks if QueryT is in the list of TargetTs
 */
template <typename QueryT, typename... TargetTs>
struct includes : std::integral_constant<bool, (std::is_same_v<QueryT, TargetTs> || ...)>
{};

template <typename QueryT, typename... TargetTs>
static constexpr bool includes_v = includes<QueryT, TargetTs...>::value;

/**
 * @brief Checks if QueryT is in the template parameter list of PackT
 */
template <typename QueryT, typename PackT> struct pack_includes;

template <typename QueryT, template <typename...> class PackTmpl, typename... PackTs>
struct pack_includes<QueryT, PackTmpl<PackTs...>> : includes<QueryT, PackTs...>
{};

template <typename QueryT, typename PackT> static constexpr bool pack_includes_v = pack_includes<QueryT, PackT>::value;

}  // namespace tyl::meta