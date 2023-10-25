/**
 * @copyright 2023-present Brian Cairl
 *
 * @file format.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdio>
#include <type_traits>

namespace tyl
{

/// C-string returned when <code>format</code> fails
template <typename CharT>
constexpr const CharT* kFormatErrorString = "::tyl::format failed (formatted_len >= BufferLen)";

/**
 * @brief Formats a string in a static buffer and returns a char pointer to the formated buffer
 *
 * @param fmt_str  format specification string
 * @param fmt_args...  args use to replace format placeholders in \c fmt_str
 *
 * @retval fmt  formatted c-string buffer
 * @retval err  c-string indicating error
 *
 * @warning holding on to the returned pointer will result in weirdness
 */
template <std::size_t BufferLen = 64UL, typename CharT, typename... FormatArgTs>
const CharT* format(const CharT* fmt_str, FormatArgTs&&... fmt_args)
{
  thread_local static CharT STATIC__fmt_buffer[BufferLen];
  if constexpr (std::is_same_v<char, CharT>)
  {
    if (std::size_t formatted_len = std::snprintf(
          STATIC__fmt_buffer, sizeof(STATIC__fmt_buffer), fmt_str, std::forward<FormatArgTs>(fmt_args)...);
        formatted_len < BufferLen)
    {
      return STATIC__fmt_buffer;
    }
  }
  else
  {
    if (std::size_t formatted_len = std::swprintf(
          STATIC__fmt_buffer, sizeof(STATIC__fmt_buffer), fmt_str, std::forward<FormatArgTs>(fmt_args)...);
        formatted_len < BufferLen)
    {
      return STATIC__fmt_buffer;
    }
  }
  return kFormatErrorString<CharT>;
}

}  // namespace tyl
