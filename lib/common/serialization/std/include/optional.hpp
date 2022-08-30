/**
 * @copyright 2022-present Brian Cairl
 *
 * @file optional.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/serialization/fwd.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

/**
 * @brief Enables trivial <code>std::optional<ValueT></code> save when <code>ValueT</code> is trivial
 */
template <typename OStreamT, typename ValueT>
struct is_trivially_serializable<binary_oarchive<OStreamT>, std::optional<ValueT>>
    : is_trivially_serializable<binary_oarchive<OStreamT>, ValueT>
{};

/**
 * @brief Enables trivial <code>std::optional<ValueT></code> load when <code>ValueT</code> is trivial
 */
template <typename IStreamT, typename ValueT>
struct is_trivially_serializable<binary_iarchive<IStreamT>, std::optional<ValueT>>
    : is_trivially_serializable<binary_iarchive<IStreamT>, ValueT>
{};


/**
 * @brief Archive-generic <code>std::optional<ValueT></code> save implementation
 */
template <typename OArchive, typename ValueT> struct save<OArchive, std::optional<ValueT>>
{
  void operator()(OArchive& ar, const std::optional<ValueT>& obj)
  {
    ar << named{"has", obj.has_value()};
    if (obj.has_value())
    {
      ar << named{"value", obj.value()};
    }
  }
};

/**
 * @brief Archive-generic <code>std::optional<ValueT></code> load implementation
 */
template <typename IArchive, typename ValueT> struct load<IArchive, std::optional<ValueT>>
{
  void operator()(IArchive& ar, std::optional<ValueT>& obj)
  {
    bool has_value;
    ar >> named{"has", has_value};
    if (has_value)
    {
      obj.emplace();
      ar >> named{"value", obj.value()};
    }
  }
};

}  // namespace tyl::serialization
