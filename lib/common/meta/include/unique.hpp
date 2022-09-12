/**
 * @copyright 2022-present Brian Cairl
 *
 * @file unique.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/meta/append.hpp>
#include <tyl/meta/includes.hpp>

namespace tyl::meta
{
namespace detail
{

template <typename OutputPackT, typename InputPackT> struct unique_impl;

template <typename OutputPackT, template <typename...> class PackTmpl, typename FirstT, typename... OtherTs>
struct unique_impl<OutputPackT, PackTmpl<FirstT, OtherTs...>>
    : std::conditional_t<
        pack_includes_v<FirstT, OutputPackT>,
        unique_impl<OutputPackT, PackTmpl<OtherTs...>>,
        unique_impl<append_t<OutputPackT, FirstT>, PackTmpl<OtherTs...>>>
{};

template <typename OutputPackT, template <typename...> class PackTmpl> struct unique_impl<OutputPackT, PackTmpl<>>
{
  using type = OutputPackT;
};

template <typename PackT> struct empty_pack;

template <template <typename...> class PackTmpl, typename... Ts> struct empty_pack<PackTmpl<Ts...>>
{
  using type = PackTmpl<>;
};

}  // namespace detail

/**
 * @brief Produces a template with a set of unique variadic arguments from a single instance of a variadic template
 */
template <typename PackT> struct unique
{
  using type = typename detail::unique_impl<typename detail::empty_pack<PackT>::type, PackT>::type;
};

template <typename PackT> using unique_t = typename unique<PackT>::type;

/**
 * @brief Produces a template with a set of unique variadic arguments from several instances of variadic templates
 */
template <typename... Packs> struct combine_unique;

template <typename FirstPack, typename SecondPack, typename... OtherPacks>
struct combine_unique<FirstPack, SecondPack, OtherPacks...>
    : combine_unique<append_t<FirstPack, SecondPack>, OtherPacks...>
{};

template <typename UniquePack> struct combine_unique<UniquePack>
{
  using type = unique_t<UniquePack>;
};

template <typename... Packs> using combine_unique_t = typename combine_unique<Packs...>::type;

}  // namespace tyl::meta