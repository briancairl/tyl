/**
 * @copyright 2022-present Brian Cairl
 *
 * @file named.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

namespace tyl::serialization
{

/**
 * @brief Forwards a value with an explicit name
 */
template <typename ValueT> struct named
{
  template <typename _ValueT>
  named(const char* _name, _ValueT&& _value) : name{_name}, value{std::forward<_ValueT>(_value)}
  {}

  const char* name;
  ValueT&& value;
};

template <typename ValueT> named(const char* _name, ValueT&& _value) -> named<ValueT>;

/**
 * @brief Archive-generic <code>named<ValueT></code> save implementation
 */
template <typename OArchive, typename ValueT> struct save<OArchive, named<ValueT>>
{
  void operator()(OArchive& ar, const named<ValueT> obj) { ar << obj.value; }
};

/**
 * @brief Archive-generic <code>named<ValueT></code> load implementation
 */
template <typename IArchive, typename ValueT> struct load<IArchive, named<ValueT>>
{
  void operator()(IArchive& ar, named<ValueT> obj) { ar >> obj.value; }
};

}  // namespace tyl::serialization
