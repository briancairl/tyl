/**
 * @copyright 2022-present Brian Cairl
 *
 * @file image.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/graphics/host/image.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>graphics::host::ImageOptions</code> serialize implementation
 */
template <typename ArchiveT> struct serialize<ArchiveT, graphics::host::ImageOptions>
{
  void operator()(ArchiveT& ar, graphics::host::ImageOptions& options)
  {
    ar& named{"channel_mode", reinterpret_cast<std::uint8_t&>(options.channel_mode)};
    ar& named{"flags", reinterpret_cast<std::uint8_t&>(options.flags)};
  }
};

}  // namespace tyl::serialization
