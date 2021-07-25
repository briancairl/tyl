#pragma once

#if __has_include(<filesystem>)

#include <filesystem>

namespace tyl
{

namespace filesystem = ::std::filesystem;

}  // namespace tyl

#else  // __has_include(<filesystem>)

#include <experimental/filesystem>

namespace tyl
{

namespace filesystem = ::std::experimental::filesystem;

}  // namespace tyl

#endif  // __has_include(<filesystem>)
