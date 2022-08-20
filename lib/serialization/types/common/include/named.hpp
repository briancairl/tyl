/**
 * @copyright 2022-present Brian Cairl
 *
 * @file named.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{
namespace detail
{

template <typename ValueT, bool Immutable> struct named;

template <typename ValueT> struct named<ValueT, true>
{
  named(const char* _name, const ValueT& _value) : name{_name}, value{_value} {}

  const char* name;
  const ValueT& value;
};

template <typename ValueT> struct named<ValueT, false>
{
  named(const char* _name, ValueT& _value) : name{_name}, value{_value} {}

  const char* name;
  ValueT& value;
};

}  // namespace detail

/**
 * @brief Forwards a value with an explicit name
 */
template <typename ValueT>
struct named : detail::named<ValueT, std::is_const_v<ValueT> or std::is_rvalue_reference_v<ValueT>>
{
  using impl = detail::named<ValueT, std::is_const_v<ValueT>>;
  using impl::impl;
};

template <typename ValueT> named(const char* _name, ValueT& _value) -> named<ValueT>;
template <typename ValueT> named(const char* _name, const ValueT& _value) -> named<const ValueT>;

template <typename T> struct is_named : std::false_type
{};

template <typename ValueT> struct is_named<named<ValueT>> : std::true_type
{};

template <typename T> static constexpr bool is_named_v = is_named<T>::value;

/**
 * @brief Archive-generic <code>named<ValueT></code> save implementation
 */
template <typename OArchive, typename ValueT> struct save<OArchive, named<ValueT>>
{
  void operator()(OArchive& ar, const named<ValueT>& obj)
  {
    ar << label{obj.name};
    ar << obj.value;
  }
};

/**
 * @brief Archive-generic <code>named<ValueT></code> load implementation
 */
template <typename IArchive, typename ValueT> struct load<IArchive, named<ValueT>>
{
  void operator()(IArchive& ar, named<ValueT> obj)
  {
    ar >> label{obj.name};
    ar >> obj.value;
  }
};

}  // namespace tyl::serialization
