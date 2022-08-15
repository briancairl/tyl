/**
 * @copyright 2022-present Brian Cairl
 *
 * @file unordered_map.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <unordered_map>

// Tyl
#include <tyl/serialization/fwd.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/types/common/named.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT></code> save implementation
 */
template <typename OArchive, typename KeyT, typename ValueT, typename HashT, typename KeyEqualT, typename AllocT>
struct save<OArchive, std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT>>
{
  void operator()(OArchive& ar, const std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT>& um)
  {
    ar << named{"size", um.size()};
    for (const auto& [key, value] : um)
    {
      ar << named{"key", key};
      ar << named{"value", value};
    }
  }
};

/**
 * @brief Archive-generic <code>std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT></code> load implementation
 */
template <typename IArchive, typename KeyT, typename ValueT, typename HashT, typename KeyEqualT, typename AllocT>
struct load<IArchive, std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT>>
{
  void operator()(IArchive& ar, std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocT>& um)
  {
    std::size_t size;
    ar >> named{"size", size};
    um.reserve(size);

    for (std::size_t i = 0; i < size; ++i)
    {
      KeyT key;
      ar >> named{"key", key};

      ValueT value;
      ar >> named{"value", value};

      um.emplace(std::move(key), std::move(value));
    }
  }
};

}  // namespace tyl::serialization
