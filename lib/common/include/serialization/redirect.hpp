/**
 * @copyright 2021-present Brian Cairl
 *
 * @file redirect.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>

// Boost
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

// Tyl
#include <tyl/serialization/typedef.hpp>

namespace tyl::serialization
{

template <typename NameT, typename ValueT> constexpr decltype(auto) field(NameT&& name, ValueT&& value)
{
  return ::boost::serialization::make_nvp(std::forward<NameT>(name), std::forward<ValueT>(value));
}

template <typename ArchiveT, typename ValueT>
constexpr void split(ArchiveT&& name, ValueT&& value, const version_t version)
{
  return ::boost::serialization::split_free(std::forward<ArchiveT>(name), std::forward<ValueT>(value), version);
}

}  // tyl::serialization
