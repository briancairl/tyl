/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

// Tyl
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{

using ImagePixelEncoding = std::uint8_t;

using ImageData = std::unique_ptr<ImagePixelEncoding[]>;

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

  /// Flip image vertically file on load
  bool flip_vertically = false;

  /// Image channel loading options
  ChannelMode channel_mode = ChannelMode::Default;

  ImageLoadData() = default;

  template <typename StringT> ImageLoadData(StringT&& _filename) : filename{std::move(_filename)} {}
};

/**
 * @brief Loads image from filesystem to a texture on device
 *
 * @returns <code>{texture, texture meta-info}</code>
 */
[[nodiscard]] std::tuple<Texture, TextureInfo>
load_to_device(const ImageLoadData& options, const TextureOptions& texture_options = {});

/**
 * @brief Loads image from filesystem to a texture on device
 *
 * @returns <code>{texture, texture meta-info}</code>
 */
[[nodiscard]] Texture
load_to_device(const ImageData& image, const TextureInfo& image_meta_info, const TextureOptions& texture_options = {});

/**
 * @brief Loads image from filesystem to image data on host
 *
 * @returns <code>{texture, texture meta-info}</code>
 */
[[nodiscard]] std::tuple<ImageData, TextureInfo> load_to_host(const ImageLoadData& options);

}  // namespace tyl::graphics
