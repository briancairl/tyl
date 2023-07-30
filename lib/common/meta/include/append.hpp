/**
 * @copyright 2022-present Brian Cairl
 *
 * @file append.hpp
 */
#pragma once

namespace tyl::meta
{

/**
 * @brief Appends AppendT to a template with a variadic parameter list
 */
template <typename PackT, typename AppendT> struct append;

template <template <typename...> class PackTmpl, typename AppendT, typename... OriginalTs>
struct append<PackTmpl<OriginalTs...>, AppendT>
{
  using type = PackTmpl<OriginalTs..., AppendT>;
};

template <template <typename...> class PackTmpl, typename... AppendTs, typename... OriginalTs>
struct append<PackTmpl<OriginalTs...>, PackTmpl<AppendTs...>>
{
  using type = PackTmpl<OriginalTs..., AppendTs...>;
};

template <typename PackT, typename AppendT> using append_t = typename append<PackT, AppendT>::type;

}  // namespace tyl::meta