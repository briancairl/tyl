/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_ostream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <cstdio>

// Tyl
#include <tyl/serialization/ostream.hpp>

namespace tyl::serialization
{

class file_ostream final : public ostream<file_ostream>
{
  friend class ostream<file_ostream>;

public:
  struct flags
  {
    std::uint8_t nobuf : 1;
    std::uint8_t append : 1;
  };

  static constexpr flags default_flags{.nobuf = true, .append = false};

  file_ostream(const char* filename, const flags fileopt = default_flags);

  file_ostream(file_ostream&& other);

  ~file_ostream();

private:
  /**
   * @copydoc ostream<file_ostream>::write
   */
  std::size_t write_impl(const void* ptr, std::size_t len) { return std::fwrite(ptr, 1, len, file_handle_); }

  /**
   * @copydoc ostream<file_ostream>::flush
   */
  void flush_impl() { std::fflush(file_handle_); }

  /// Native file handle
  std::FILE* file_handle_ = nullptr;
};

}  // namespace tyl::serialization
