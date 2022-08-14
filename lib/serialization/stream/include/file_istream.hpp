/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_istream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <cstdio>

// Tyl
#include <tyl/serialization/istream.hpp>

namespace tyl::serialization
{

class file_istream final : public istream<file_istream>
{
  friend class istream<file_istream>;

public:
  struct flags
  {
    std::uint8_t nobuf : 1;
  };

  static constexpr flags default_flags{.nobuf = true};

  file_istream(const char* filename, const flags fileopt = default_flags);

  file_istream(file_istream&& other);

  ~file_istream();

private:
  /**
   * @copydoc istream<file_istream>::read
   */
  std::size_t read_impl(void* ptr, std::size_t len)
  {
    const std::size_t read_bytes = std::fread(ptr, 1, len, file_handle_);
    file_bytes_remaining_ -= read_bytes;
    return read_bytes;
  }

  /**
   * @copydoc istream<file_istream>::available
   */
  std::size_t available_impl() const { return file_bytes_remaining_; }

  /// Native file handle
  std::FILE* file_handle_ = nullptr;

  /// Number of remaining bytes in file
  std::size_t file_bytes_remaining_ = 0;
};

}  // namespace tyl::serialization
