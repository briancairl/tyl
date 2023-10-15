/**
 * @copyright 2022-present Brian Cairl
 *
 * @file istream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/utility/crtp.hpp>

namespace tyl::serialization
{

template <typename IStreamT> class istream : public crtp_base<istream<IStreamT>>
{
public:
  /**
   * @brief Reads bytes from the stream
   */
  constexpr std::size_t read(void* ptr, std::size_t len) { return this->derived().read_impl(ptr, len); }

  /**
   * @brief Reads stream to C-style array
   */
  template <typename ElementT, std::size_t ElementCount> constexpr std::size_t read(ElementT (&array)[ElementCount])
  {
    return istream::read(reinterpret_cast<void*>(array), sizeof(ElementT) * ElementCount);
  }

  /**
   * @brief Checks the next byte in the stream without changing the state of the stream
   */
  constexpr decltype(auto) peek() { return this->derived().peek_impl(); }

  /**
   * @brief Returns number of available bytes left in the stream
   */
  constexpr std::size_t available() const { return this->derived().available_impl(); }

  istream() = default;

private:
  istream(const istream&) = default;

  static constexpr void flush_impl()
  { /*default*/
  }
};

}  // namespace tyl::serialization
