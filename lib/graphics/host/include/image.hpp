/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/graphics/device/fwd.hpp>

namespace tyl::graphics::host
{

/**
 * @brief Image load options
 */
struct ImageOptions
{
  enum class ChannelMode : std::uint8_t
  {
    Default,
    Grey,
    GreyA,
    RGB,
    RGBA
  };

  /// Image channel loading options
  ChannelMode channel_mode = ChannelMode::Default;

  /// On-load option flags
  struct
  {
    std::uint8_t flip_vertically : 1;
  } flags = {0};
};

/**
 * @brief Loads image from filesystem to image data on host
 *
 * @param path  path to image file
 * @param options  image loading options
 *
 * @return host-side texture data
 */
[[nodiscard]] device::TextureHost load(const char* path, const ImageOptions& options = ImageOptions{});

}  // namespace tyl::graphics::host
