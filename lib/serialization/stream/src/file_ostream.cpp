/**
 * @copyright 2022-present Brian Cairl
 *
 * @file file_ostream.cpp
 */

// C++ Standard Library
#include <stdexcept>

// Tyl
#include <tyl/serialization/file_ostream.hpp>

namespace tyl::serialization
{
namespace  // anonymous
{

const char* flags_to_write_mode_str(file_ostream::flags fileopt)
{
  if (fileopt.append)
  {
    return "ab";
  }
  else
  {
    return "wb";
  }
}

const char* flags_to_write_mode_str_human_readable(file_ostream::flags fileopt)
{
  if (fileopt.append)
  {
    return "append";
  }
  else
  {
    return "write";
  }
}

}  // namespace anonymous

file_ostream::file_ostream(const char* filename, const flags fileopt) :
    file_handle_{std::fopen(filename, flags_to_write_mode_str(fileopt))}
{
  if (file_handle_ == nullptr)
  {
    static char errbuf[64];
    if (
      std::snprintf(
        errbuf,
        sizeof(errbuf),
        "failed to to open file (%s) for %s",
        filename,
        flags_to_write_mode_str_human_readable(fileopt)) < 0)
    {
      errbuf[sizeof(errbuf) - 1] = '\0';
    }
    throw std::runtime_error{errbuf};
  }

  if (fileopt.nobuf)
  {
    std::setvbuf(file_handle_, nullptr, _IONBF, 0);
  }
}

file_ostream::file_ostream(file_ostream&& other) : file_handle_{other.file_handle_} { other.file_handle_ = nullptr; }

file_ostream::~file_ostream()
{
  if (file_handle_ == nullptr)
  {
    return;
  }
  std::fclose(file_handle_);
}

}  // namespace tyl::serialization