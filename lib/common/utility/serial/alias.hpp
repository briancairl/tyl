/**
 * @copyright 2022-present Brian Cairl
 *
 * @file alias.hpp
 */
#pragma once

// Tyl
#include <tyl/serial.hpp>
#include <tyl/utility/alias.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>alias<T, Tag></code> serialize implementation
 */
template <typename Archive, typename T, typename Tag> struct serialize<Archive, alias<T, Tag>>
{
  void operator()(Archive& ar, alias<T, Tag>& aliased) { ar& named{"value", aliased.value}; }
};

}  // namespace tyl::serialization
