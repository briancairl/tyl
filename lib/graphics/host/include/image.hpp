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

namespace tyl::graphics
{

/**
 * @brief Data used to describe an image to load from disk
 */
struct ImageLoadData
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

  /// Flip image vertically file on load
  bool flip_vertically = true;
};

/**
 * @brief Loads image from filesystem to image data on host
 *
 * @return host-side texture data
 */
[[nodiscard]] device::TextureHost load(const ImageLoadData& options);

}  // namespace tyl::graphics
