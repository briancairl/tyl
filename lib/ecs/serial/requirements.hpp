/**
 * @copyright 2022-present Brian Cairl
 *
 * @file load.hpp
 */
#pragma once

// C++ Standard Library
#include <iterator>
#include <type_traits>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/reflection/typestr.hpp>
#include <tyl/serialization/iarchive.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/sequence.hpp>

namespace tyl::ecs
{
namespace detail
{

template <typename T, typename Pack> struct includes : std::false_type
{};

template <typename T, template <typename...> VariadicTmpl, typename... VariadicTs>
struct includes<T, VariadicTmpl<VariadicTs...>>
    : std::conditional_t<(std::is_same_v<T, VariadicTs>() || ...), std::true_type, std::false_type>
{};

template <template <typename...> VariadicTmpl, typename... Ts> struct combine_unique;

template <template <typename> VariadicTmpl, typename T> struct combine_unique<VariadicTmpl<T>>
{
  using type = VariadicTmpl<T>;
};

template <template <typename...> VariadicTmpl, typename... LHS_Ts, typename RHS_CandidateT, typename... RHS_OtherTs>
struct combine_unique<VariadicTmpl<LHS_Ts...>, VariadicTmpl<RHS_CandidateT, RHS_OtherTs...>>
{
  using type = std::conditional_t<
    includes<RHS_CandidateT, VariadicTmpl<LHS_Ts...>>::value,

    >;
};

template <template <typename...> VariadicTmpl, typename... LHS_Ts, typename... RHS_Ts>
struct combine_unique<VariadicTmpl<LHS_Ts...>, VariadicTmpl<RHS_Ts...>>
{
  using type = VariadicTmpl<T>;
};

};

template <typename... RequiredComponentsTs> struct required_components
{};

template <typename... Ts> struct combine_required_components;

template <typename... RequiredComponentsTs>
struct combine_required_components<required_components<RequiredComponentsTs...>>
{
  using type = required_components<RequiredComponentsTs...>;
};

template <typename... LHSRequiredComponentsTs, typename... RHSRequiredComponentsTs>
struct combine_required_components<
  required_components<LHSRequiredComponentsTs...>,
  required_components<RHSRequiredComponentsTs...>>
{
  using type = ;
};

}  // namespace tyl::ecs

namespace tyl::serialization
{}  // namespace tyl::serialization
