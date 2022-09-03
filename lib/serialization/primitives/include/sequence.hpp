/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sequence.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

namespace tyl::serialization
{

template <typename IteratorT> struct sequence
{
  IteratorT first;
  IteratorT last;
  constexpr IteratorT begin() const { return first; }
  constexpr IteratorT end() const { return last; }
  constexpr bool empty() const { return first == last; }
};

template <typename IteratorT> constexpr sequence<IteratorT> make_sequence(IteratorT first, IteratorT last)
{
  return sequence<IteratorT>{first, last};
}

template <typename T> struct is_sequence : std::false_type
{};

template <typename IteratorT> struct is_sequence<sequence<IteratorT>> : std::true_type
{};

template <typename T>
static constexpr bool is_sequence_v = is_sequence<std::remove_const_t<std::remove_reference_t<T>>>::value;

}  // namespace tyl::serialization
