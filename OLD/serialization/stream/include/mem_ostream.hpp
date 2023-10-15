/**
 * @copyright 2022-present Brian Cairl
 *
 * @file mem_ostream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <cstring>
#include <vector>

// Tyl
#include <tyl/serialization/ostream.hpp>

namespace tyl::serialization
{

class mem_ostream final : public ostream<mem_ostream>
{
  friend class ostream<mem_ostream>;
  friend class mem_istream;

public:
  mem_ostream(const std::size_t initial_capacity = 64UL);

  mem_ostream(mem_ostream&& other);

  ~mem_ostream();

private:
  /**
   * @copydoc ostream<mem_ostream>::write
   */
  std::size_t write_impl(const void* ptr, std::size_t len)
  {
    const std::size_t pos = buffer_.size();
    buffer_.resize(buffer_.size() + len);
    std::memcpy(buffer_.data() + pos, ptr, len);
    return len;
  }

  /// Byte stream buffer
  std::vector<std::uint8_t> buffer_ = {};
};

}  // namespace tyl::serialization
