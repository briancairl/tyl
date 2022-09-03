/**
 * @copyright 2022-present Brian Cairl
 *
 * @file typestr.hpp
 */
#pragma once

// C++ Standard Library
#include <cstring>
#include <string_view>

namespace tyl
{

/**
 * @brief Returns the full-qualified name of a type as a C-style string
 */
template <typename T> const char* typestr()
{
#if 1  // defined(__clang__) or defined(__gcc__)
  static constexpr std::string_view SIGNATURE{__PRETTY_FUNCTION__};
  static constexpr std::string_view TOKEN{"T = "};
  static constexpr std::size_t OFFSET = SIGNATURE.find(TOKEN);
  static char typestr_storage[SIGNATURE.size() - TOKEN.size() - OFFSET] = {'\0'};
  if (typestr_storage[0] == '\0')
  {
    std::memcpy(typestr_storage, SIGNATURE.data() + TOKEN.size() + OFFSET, SIGNATURE.size() - TOKEN.size() - OFFSET);
    typestr_storage[SIGNATURE.size() - TOKEN.size() - OFFSET - 1] = '\0';
  }
  return typestr_storage;
#else  // __PRETTY_FUNCTION__
  return "typestr<T> unsupported";
#endif  // __PRETTY_FUNCTION__
}

}  // namespace tyl
