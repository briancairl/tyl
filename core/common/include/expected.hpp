/**
 * @copyright 2021-present Brian Cairl
 *
 * @file expected.hpp
 */
#pragma once

#ifdef __cpp_lib_expected
// C++ Standard Library
#include <expected>
#define TYL_CORE_COMMON_EXPECTED_NAMESPACE ::std
#else  // __cpp_lib_expected
// TartanLlama
#include <tl/expected.hpp>
#define TYL_CORE_COMMON_EXPECTED_NAMESPACE ::tl
#endif  // __cpp_lib_expected

namespace tyl
{

template <typename T, typename E> using expected = TYL_CORE_COMMON_EXPECTED_NAMESPACE::expected<T, E>;

template <typename E> using unexpected = TYL_CORE_COMMON_EXPECTED_NAMESPACE::unexpected<E>;

template <class E> decltype(auto) make_unexpected(E&& e)
{
  return TYL_CORE_COMMON_EXPECTED_NAMESPACE::make_unexpected(std::forward<E>(e));
}

}  // namespace tyl
