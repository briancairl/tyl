/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_ostream.hpp
 */
#pragma once

// C++ Standard Library
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>

// Tyl
#include <tyl/serialization/ostream.hpp>

namespace tyl::serialization
{

class file_handle_ostream : public ostream<file_handle_ostream>
{
  friend class ostream<file_handle_ostream>;

public:
  explicit file_handle_ostream(std::FILE* file_handle) : file_handle_{file_handle} {}

  file_handle_ostream(file_handle_ostream&& other) : file_handle_{other.file_handle_} { other.file_handle_ = nullptr; }

private:
  /**
   * @copydoc ostream<file_ostream>::write
   */
  std::size_t write_impl(const void* ptr, std::size_t len)
  {
    return std::fwrite(ptr, sizeof(std::byte), len, file_handle_);
  }

  /**
   * @copydoc ostream<file_ostream>::flush
   */
  void flush_impl() { std::fflush(file_handle_); }

protected:
  /// Native file handle
  std::FILE* file_handle_ = nullptr;
};


class file_ostream final : public file_handle_ostream
{
public:
  struct flags
  {
    std::uint8_t nobuf : 1;
    std::uint8_t append : 1;
    std::uint8_t binary : 1;
  };

  static constexpr flags default_flags{.nobuf = true, .append = false, .binary = true};

  explicit file_ostream(const char* filename, const flags fileopt = default_flags);

  explicit file_ostream(const std::filesystem::path& path, const flags fileopt = default_flags) :
      file_ostream{path.c_str(), fileopt}
  {}

  file_ostream(file_ostream&& other) = default;

  ~file_ostream();
};

}  // namespace tyl::serialization
