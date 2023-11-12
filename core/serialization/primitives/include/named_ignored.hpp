/**
 * @copyright 2023-present Brian Cairl
 *
 * @file named_ignored.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/serialization/label.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{
namespace detail
{

template <typename ValueT, bool Immutable> struct named_ignored;

template <typename ValueT> struct named_ignored<ValueT, true>
{
  named_ignored(const char* _name, [[maybe_unused]] const ValueT& _) : name{_name} {}

  const char* name;
};

template <typename ValueT> struct named_ignored<ValueT, false>
{
  named_ignored(const char* _name, ValueT& value_to_default) : name{_name} { value_to_default = ValueT{}; }

  const char* name;
};

}  // namespace detail

/**
 * @brief Forwards a value with an explicit name
 */
template <typename ValueT>
struct named_ignored : detail::named_ignored<ValueT, std::is_const_v<ValueT> or std::is_rvalue_reference_v<ValueT>>
{
  using impl = detail::named_ignored<ValueT, std::is_const_v<ValueT>>;
  using impl::impl;
};

template <typename ValueT> named_ignored(const char* _name, ValueT& _value) -> named_ignored<ValueT>;
template <typename ValueT> named_ignored(const char* _name, const ValueT& _value) -> named_ignored<const ValueT>;

template <typename T> struct is_named_ignored : std::false_type
{};

template <typename ValueT> struct is_named_ignored<named_ignored<ValueT>> : std::true_type
{};

template <typename T> static constexpr bool is_named_ignored_v = is_named_ignored<T>::value;

/**
 * @brief Archive-generic <code>named_ignored<ValueT></code> save implementation
 */
template <typename OArchive, typename ValueT> struct save<OArchive, named_ignored<ValueT>>
{
  void operator()(OArchive& ar, const named_ignored<ValueT>& obj) { ar << label{obj.name}; }
};

/**
 * @brief Archive-generic <code>named_ignored<ValueT></code> load implementation
 */
template <typename IArchive, typename ValueT> struct load<IArchive, named_ignored<ValueT>>
{
  void operator()(IArchive& ar, named_ignored<ValueT> obj) { ar >> label{obj.name}; }
};

}  // namespace tyl::serialization
