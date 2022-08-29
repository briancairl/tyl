/**
 * @copyright 2022-present Brian Cairl
 *
 * @file mem_istream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <cstring>
#include <vector>

// Tyl
#include <tyl/serialization/istream.hpp>

namespace tyl::serialization
{

class mem_ostream;

class mem_istream final : public istream<mem_istream>
{
  friend class istream<mem_istream>;

public:
  mem_istream(std::vector<std::uint8_t>&& buffer);

  mem_istream(mem_istream&& other);

  mem_istream(mem_ostream&& other);

  ~mem_istream();

private:
  /**
   * @copydoc istream<mem_istream>::read
   */
  std::size_t read_impl(void* ptr, std::size_t len)
  {
    if (len + pos_ < buffer_.size())
    {
      std::memcpy(ptr, buffer_.data() + pos_, len);
      pos_ += len;
      return len;
    }
    else
    {
      return 0;
    }
  }

  /**
   * @copydoc istream<mem_istream>::peek
   */
  char peek_impl() { return buffer_[pos_]; }

  /**
   * @copydoc istream<mem_istream>::available
   */
  std::size_t available_impl() const { return buffer_.size() - pos_; }

  /// Byte stream buffer
  std::vector<std::uint8_t> buffer_ = {};

  /// Current read-byte position
  std::size_t pos_ = 0;
};

}  // namespace tyl::serialization
