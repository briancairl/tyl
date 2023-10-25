/**
 * @copyright 2023-present Brian Cairl
 *
 * @file tuple.hpp
 */
#pragma once

// C++ Standard Library
#include <tuple>
#include <utility>

// Tyl
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{
namespace detail
{

constexpr std::tuple kTupleKeys{
  "el0",
  "el1",
  "el2",
  "el3",
  "el4",
  "el5",
  "el6",
  "el7",
  "el8",
  "el9",
};

}  // namespace

template <typename OArchiveT, typename... Ts> struct save<OArchiveT, std::tuple<Ts...>>
{
  void operator()(OArchiveT& oar, const std::tuple<Ts...>& tup)
  {
    if constexpr ((is_trivially_serializable_v<OArchiveT, Ts> && ...))
    {
      oar << named{"data", make_packet(&tup)};
    }
    else
    {
      expand(oar, tup, std::make_integer_sequence<std::size_t, sizeof...(Ts)>{});
    }
  }

private:
  template <typename Tup, std::size_t... Is>
  static int expand(OArchiveT& oar, Tup&& tup, std::integer_sequence<std::size_t, Is...> _)
  {
    return ((oar << named{std::get<Is>(detail::kTupleKeys), std::get<Is>(std::forward<Tup>(tup))}, 0) + ...);
  }
};

template <typename IArchiveT, typename... Ts> struct load<IArchiveT, std::tuple<Ts...>>
{
  void operator()(IArchiveT& iar, std::tuple<Ts...>& tup)
  {
    if constexpr ((is_trivially_serializable_v<IArchiveT, Ts> && ...))
    {
      iar >> named{"data", make_packet(&tup)};
    }
    else
    {
      expand(iar, tup, std::make_integer_sequence<std::size_t, sizeof...(Ts)>{});
    }
  }

private:
  template <typename Tup, std::size_t... Is>
  static int expand(IArchiveT& iar, Tup&& tup, std::integer_sequence<std::size_t, Is...> _)
  {
    return ((iar >> named{std::get<Is>(detail::kTupleKeys), std::get<Is>(std::forward<Tup>(tup))}, 0) + ...);
  }
};

}  // namespace tyl::serialization
