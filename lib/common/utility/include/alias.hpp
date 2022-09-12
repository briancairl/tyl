/**
 * @copyright 2022-present Brian Cairl
 *
 * @file alias.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

namespace tyl
{

template <char... Name> struct alias_tag
{
  constexpr static const char name_storage[] = {Name..., '\0'};
  static constexpr const char* name() { return name_storage; }
};

template <typename Tag> struct is_alias_tag : std::false_type
{};

template <char... Name> struct is_alias_tag<alias_tag<Name...>> : std::true_type
{};

template <typename T, T... Name> constexpr alias_tag<Name...> operator""_tag() { return alias_tag<Name...>{}; }

/**
 * @brief Strong type alias
 *
 * @tparam T    type to alias
 * @tparam Tag  unique identifier type
 */
template <typename T, typename Tag> struct alias
{
  static_assert(is_alias_tag<Tag>());

  template <typename... CTorArgs> explicit alias(CTorArgs&&... ctor_args) : value{std::forward<CTorArgs>(ctor_args)...}
  {}

  constexpr operator T&() { return value; }
  constexpr operator const T&() const { return value; }

  constexpr T& operator*() { return value; }
  constexpr const T& operator*() const { return value; }

  constexpr T* operator->() { return &value; }
  constexpr const T* operator->() const { return &value; }

  template <typename AssignT> constexpr T& operator=(AssignT&& assign) { value = std::forward<AssignT>(assign); }

  template <typename IndexT> constexpr decltype(auto) operator[](IndexT&& pos)
  {
    return value[std::forward<IndexT>(pos)];
  }

  template <typename IndexT> constexpr decltype(auto) operator[](IndexT&& pos) const
  {
    return value[std::forward<IndexT>(pos)];
  }

  T value;
};

template <typename AliasT> struct underlying;

template <typename T, typename Tag> struct underlying<alias<T, Tag>>
{
  using type = T;
};

template <typename AliasT> using underlying_t = typename underlying<AliasT>::type;

template <typename AliasT> constexpr underlying<AliasT> to_underlying(const AliasT& alias) { return *alias; }

template <typename T, typename Tag> constexpr bool operator==(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) == (*rhs);
}

template <typename T, typename Tag> constexpr bool operator!=(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) != (*rhs);
}

template <typename T, typename Tag> constexpr bool operator==(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs == (*rhs);
}

template <typename T, typename Tag> constexpr bool operator!=(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs != (*rhs);
}

template <typename T, typename Tag> constexpr bool operator==(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) == rhs;
}

template <typename T, typename Tag> constexpr bool operator!=(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) != rhs;
}

template <typename T, typename Tag> constexpr bool operator<(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) < (*rhs);
}

template <typename T, typename Tag> constexpr bool operator>(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) > (*rhs);
}

template <typename T, typename Tag> constexpr bool operator<(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs < (*rhs);
}

template <typename T, typename Tag> constexpr bool operator>(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs > (*rhs);
}

template <typename T, typename Tag> constexpr bool operator<(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) < rhs;
}

template <typename T, typename Tag> constexpr bool operator>(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) > rhs;
}

template <typename T, typename Tag> constexpr bool operator<=(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) <= (*rhs);
}

template <typename T, typename Tag> constexpr bool operator>=(const alias<T, Tag>& lhs, const alias<T, Tag>& rhs)
{
  return (*lhs) >= (*rhs);
}

template <typename T, typename Tag> constexpr bool operator<=(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs <= (*rhs);
}

template <typename T, typename Tag> constexpr bool operator>=(const T& lhs, const alias<T, Tag>& rhs)
{
  return lhs >= (*rhs);
}

template <typename T, typename Tag> constexpr bool operator<=(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) <= rhs;
}

template <typename T, typename Tag> constexpr bool operator>=(const alias<T, Tag>& lhs, const T& rhs)
{
  return (*lhs) >= rhs;
}

}  // namespace tyl
