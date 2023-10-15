/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_istream.cpp
 */

// C++ Standard Library
#include <stdexcept>

// Tyl
#include <tyl/serialization/file_istream.hpp>

namespace tyl::serialization
{

file_istream::file_istream(const char* filename, const flags fileopt) : file_handle_{std::fopen(filename, "rb")}
{
  if (file_handle_ == nullptr)
  {
    char errbuf[128];
    if (std::snprintf(errbuf, sizeof(errbuf), "failed to to open file (%s) for read", filename) < 0)
    {
      errbuf[sizeof(errbuf) - 1] = '\0';
    }
    throw std::runtime_error{errbuf};
  }

  file_bytes_remaining_ = [file = file_handle_] {
    std::fseek(file, 0, SEEK_END);
    const auto size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    return size;
  }();

  if (fileopt.nobuf)
  {
    std::setvbuf(file_handle_, nullptr, _IONBF, 0);
  }
}

file_istream::file_istream(file_istream&& other) :
    file_handle_{other.file_handle_}, file_bytes_remaining_{other.file_bytes_remaining_}
{
  other.file_handle_ = nullptr;
  other.file_bytes_remaining_ = 0;
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