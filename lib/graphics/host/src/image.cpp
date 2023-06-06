/**
 * @copyright 2022-present Brian Cairl
 *
 * @file image.cpp
 */

// C++ Standard Library
#include <cstdint>
#include <cstring>
#include <type_traits>

// STB
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

// Tyl
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>

namespace tyl::graphics::host
{
namespace  // anonymous
{

int channel_mode_to_stbi_enum(const ImageOptions::ChannelMode mode)
{
  switch (mode)
  {
  case ImageOptions::ChannelMode::Default:
    return 0;
  case ImageOptions::ChannelMode::Grey:
    return 1;
  case ImageOptions::ChannelMode::GreyA:
    return 2;
  case ImageOptions::ChannelMode::RGB:
    return 3;
  case ImageOptions::ChannelMode::RGBA:
    return 4;
  default:
    break;
  }
  return 0;
}

device::TextureChannels image_channel_count_to_texture_mode(const int count)
{
  switch (count)
  {
  case 1:
    return device::TextureChannels::R;
  case 2:
    return device::TextureChannels::RG;
  case 3:
    return device::TextureChannels::RGB;
  case 4:
    return device::TextureChannels::RGBA;
  default:
    break;
  }
  return device::TextureChannels::R;
}

}  // namespace anonymous

Image::Image(const ImageMetaData& meta, void* const data) : meta_{meta}, data_{data} {}

Image::Image(Image&& other) : meta_{other.meta_}, data_{other.data_} { other.data_ = nullptr; }

Image::~Image()
{
  if (data_ == nullptr)
  {
    return;
  }
  else
  {
    std::free(data_);
  }
}

device::TextureHost Image::texture() const noexcept
{
  return device::TextureHost{
    data_,
    meta_.height,
    meta_.width,
    device::TypeCode::UInt8,
    image_channel_count_to_texture_mode(meta_.channel_count)};
}

tyl::expected<Image, ImageErrorCode> Image::load(const char* path, const ImageOptions& options) noexcept
{
  // Set flag determining whether image should be flipped on load
  stbi_set_flip_vertically_on_load(options.flags.flip_vertically);

  // Get STBI channel code
  const int channel_count_forced = channel_mode_to_stbi_enum(options.channel_mode);

  // Load image data and sizing
  ImageMetaData meta;
  auto* image_data_ptr = stbi_load(path, &meta.height, &meta.width, &meta.channel_count, channel_count_forced);

  static_assert(std::is_same<decltype(image_data_ptr), std::uint8_t*>(), "Image data not loaded as byte array");

  // Check if image point is valid
  if (image_data_ptr == nullptr)
  {
    return tyl::unexpected{ImageErrorCode::LOAD_FAILURE};
  }

  // Resolve number of channels if channel count was forced with 'options'
  if (options.channel_mode != ImageOptions::ChannelMode::Default)
  {
    meta.channel_count = channel_count_forced;
  }

  return Image{meta, image_data_ptr};
}

}  // namespace tyl::graphics::host
