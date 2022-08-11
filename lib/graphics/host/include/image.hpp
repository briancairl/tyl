/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <string>

// Tyl
#include <tyl/graphics/device/fwd.hpp>

namespace tyl::graphics::host
{

/**
 * @brief Descriptor used to describe an image to load from disk
 */
struct Image
{
  enum class ChannelMode
  {
    Default,
    Grey,
    GreyA,
    RGB,
    RGBA
  };

  /// Image filename
  std::string filename;

  /// Image channel loading options
  ChannelMode channel_mode = ChannelMode::Default;

  /// On-load option flags
  struct
  {
    std::uint8_t flip_vertically : 1;
  } flags = {};
};

/**
 * @brief Loads image from filesystem to image data on host
 *
 * @return host-side texture data
 */
[[nodiscard]] device::TextureHost load(const Image& options);

}  // namespace tyl::graphics::host
