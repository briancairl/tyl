/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.cpp
 */

// C++ Standard Library
#include <cstdint>
#include <sstream>
#include <stdexcept>

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
#include <tyl/device/graphics/texture.hpp>
#include <tyl/graphics/image.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

int channel_mode_to_stbi_enum(const ImageLoadData::ChannelMode mode)
{
  switch (mode)
  {
  case ImageLoadData::ChannelMode::Default:
    return 0;
  case ImageLoadData::ChannelMode::Grey:
    return 1;
  case ImageLoadData::ChannelMode::GreyA:
    return 2;
  case ImageLoadData::ChannelMode::RGB:
    return 3;
  case ImageLoadData::ChannelMode::RGBA:
    return 4;
  default:
    break;
  }
  return 0;
}

TextureChannels image_channel_count_to_mode(const int count)
{
  switch (count)
  {
  case 1:
    return TextureChannels::R;
  case 2:
    return TextureChannels::RG;
  case 3:
    return TextureChannels::RGB;
  case 4:
    return TextureChannels::RGBA;
  default:
    break;
  }
  return TextureChannels::R;
}

}  // namespace anonymous

std::tuple<ImageData, TextureInfo> load_to_host(const ImageLoadData& options)
{
  TextureInfo image_meta_info;

  stbi_set_flip_vertically_on_load(options.flip_vertically);

  // Load image data and sizing
  auto* image_data_ptr = stbi_load(
    options.filename.c_str(),
    &image_meta_info.size.x(),
    &image_meta_info.size.y(),
    &image_meta_info.channels,
    channel_mode_to_stbi_enum(options.channel_mode));

  static_assert(std::is_same<decltype(image_data_ptr), ImagePixelEncoding*>(), "Image data not loaded as byte array");

  // Check if image point is valid
  if (image_data_ptr == nullptr)
  {
    std::ostringstream oss;
    oss << "Failed to load image " << options.filename << " : " << stbi_failure_reason();
    throw std::runtime_error{oss.str()};
  }

  return std::make_tuple(ImageData{image_data_ptr}, std::move(image_meta_info));
}

Texture
load_to_device(const ImageData& image, const TextureInfo& image_meta_info, const TextureOptions& texture_options)
{
  return Texture{
    image_meta_info.size.x(),
    image_meta_info.size.y(),
    image.get(),
    image_channel_count_to_mode(image_meta_info.channels),
    texture_options};
}

std::tuple<Texture, TextureInfo> load_to_device(const ImageLoadData& options, const TextureOptions& texture_options)
{
  auto [image_data_ptr, image_meta_info] = load_to_host(options);

  return std::make_tuple(load_to_device(image_data_ptr, image_meta_info, texture_options), image_meta_info);
}

}  // namespace tyl::graphics
