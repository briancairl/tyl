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
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/image.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

int channel_mode_to_stbi_enum(const Image::ChannelMode mode)
{
  switch (mode)
  {
  case Image::ChannelMode::Default:
    return 0;
  case Image::ChannelMode::Grey:
    return 1;
  case Image::ChannelMode::GreyA:
    return 2;
  case Image::ChannelMode::RGB:
    return 3;
  case Image::ChannelMode::RGBA:
    return 4;
  default:
    break;
  }
  return 0;
}

device::TextureChannels image_channel_count_to_mode(const int count)
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

device::TextureHost load(const Image& image_options)
{
  stbi_set_flip_vertically_on_load(image_options.flags.flip_vertically);

  const int c_forced = channel_mode_to_stbi_enum(image_options.channel_mode);

  // Load image data and sizing
  int w, h, c;
  auto* image_data_ptr = stbi_load(image_options.filename.c_str(), &h, &w, &c, c_forced);

  static_assert(std::is_same<decltype(image_data_ptr), std::uint8_t*>(), "Image data not loaded as byte array");

  // Check if image point is valid
  if (image_data_ptr == nullptr)
  {
    std::ostringstream oss;
    oss << "Failed to load image " << image_options.filename << " : " << stbi_failure_reason();
    throw std::runtime_error{oss.str()};
  }

  const int c_resolved = (image_options.channel_mode == Image::ChannelMode::Default) ? c : c_forced;

  return device::TextureHost{
    std::unique_ptr<std::uint8_t[]>{image_data_ptr},
    h,
    w,
    device::TypeCode::UInt8,
    image_channel_count_to_mode(c_resolved)};
}

}  // namespace tyl::graphics