/**
 * @copyright 2022-present Brian Cairl
 *
 * @file ostream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

namespace tyl::serialization
{

template <typename OStreamT> class ostream
{
public:
  /**
   * @brief Writes bytes to the stream
   */
  constexpr std::size_t write(const void* ptr, std::size_t len) { return ostream::derived()->write_impl(ptr, len); }

  /**
   * @brief Writes C-style array to the stream
   */
  template <typename ElementT, std::size_t ElementCount>
  constexpr std::size_t write(const ElementT (&array)[ElementCount])
  {
    return ostream::write(reinterpret_cast<const void*>(array), sizeof(ElementT) * ElementCount);
  }

  /**
   * @brief Flushes buffered bytes to target
   */
  constexpr void flush() { return ostream::derived()->flush_impl(); }

  ostream() = default;

private:
  ostream(const ostream&) = default;

  constexpr OStreamT* derived() { return static_cast<OStreamT*>(this); }

  constexpr const OStreamT* derived() const { return static_cast<const OStreamT*>(this); }

  static constexpr void flush_impl()
  { /*default*/
  }
};

}  // namespace tyl::serialization
