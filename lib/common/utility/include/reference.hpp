/**
 * @copyright 2022-present Brian Cairl
 *
 * @file handle.hpp
 */
#pragma once

// C++ Standard Library
#include <utility>

namespace tyl
{

/**
 * @brief Holds a type and an absoluted ID
 */
template <typename IdentifierT, typename ValueT = void> class Reference
{
public:
  template <typename... ArgTs>
  explicit Reference(IdentifierT id, ArgTs&&... args) : id_{id}, value_{std::forward<ArgTs>(args)...}
  {}

  constexpr IdentifierT id() const { return id_; }

  constexpr ValueT& value() { return value_; }
  constexpr const ValueT& value() const { return value_; }

  constexpr ValueT& operator*() { return value_; }
  constexpr const ValueT& operator*() const { return value_; }

  constexpr ValueT* operator->() { return std::addressof(value_); }
  constexpr const ValueT* operator->() const { return std::addressof(value_); }

private:
  IdentifierT id_;
  ValueT value_;
};

/**
 * @brief Holds a type and an absoluted ID
 */
template <typename IdentifierT> class Reference<IdentifierT, void>
{
public:
  template <typename... ArgTs> explicit Reference(IdentifierT id) : id_{id} {}

  constexpr IdentifierT id() const { return id_; }

private:
  IdentifierT id_;
};


}  // namespace tyl