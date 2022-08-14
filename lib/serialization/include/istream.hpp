/**
 * @copyright 2022-present Brian Cairl
 *
 * @file istream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::serialization
{

template <typename IStreamT> class istream
{
public:
  /**
   * @brief Reads bytes from the stream
   */
  constexpr std::size_t read(void* ptr, std::size_t len) { return istream::derived()->read_impl(ptr, len); }

  /**
   * @brief Reads stream to C-style array
   */
  template <typename ElementT, std::size_t ElementCount> constexpr std::size_t read(ElementT (&array)[ElementCount])
  {
    return istream::read(reinterpret_cast<void*>(array), sizeof(ElementT) * ElementCount);
  }

  /**
   * @brief Returns number of available bytes left in the stream
   */
  constexpr std::size_t available() const { return istream::derived()->available_impl(); }

  istream() = default;

private:
  istream(const istream&) = default;

  constexpr IStreamT* derived() { return static_cast<IStreamT*>(this); }

  constexpr const IStreamT* derived() const { return static_cast<const IStreamT*>(this); }

  static constexpr void flush_impl()
  { /*default*/
  }
};

}  // namespace tyl::serialization
