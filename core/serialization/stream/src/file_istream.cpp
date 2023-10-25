/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_istream.cpp
 */

// C++ Standard Library
#include <stdexcept>

// Tyl
#include <tyl/format.hpp>
#include <tyl/serialization/file_istream.hpp>

namespace tyl::serialization
{
namespace  // anonymous
{

const char* flags_to_read_mode_str(file_istream::flags fileopt) { return fileopt.binary ? "rb" : "r"; }

const char* flags_to_read_mode_str_human_readable(file_istream::flags fileopt)
{
  return fileopt.binary ? "read|binary" : "read";
}

}  // namespace anonymous

file_handle_istream::file_handle_istream(std::FILE* file_handle) : file_bytes_remaining_{0}, file_handle_{file_handle}
{
  file_bytes_remaining_ = [file = file_handle_] {
    std::fseek(file, 0, SEEK_END);
    const auto size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    return size;
  }();
}

file_istream::file_istream(const char* filename, const flags fileopt) :
    file_handle_istream{std::fopen(filename, flags_to_read_mode_str(fileopt))}
{
  if (file_handle_ == nullptr)
  {
    throw std::runtime_error{format<32UL>(
      "failed to to open file (%s) for read for mode %s", filename, flags_to_read_mode_str_human_readable(fileopt))};
  }

  if (fileopt.nobuf)
  {
    std::setvbuf(file_handle_, nullptr, _IONBF, 0);
  }
}

file_istream::~file_istream()
{
  if (file_handle_ == nullptr)
  {
    return;
  }
  std::fclose(file_handle_);
}

}  // namespace tyl::serialization