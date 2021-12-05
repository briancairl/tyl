/**
 * @copyright 2021-present Brian Cairl
 *
 * @file texture.cpp
 */

// Tyl
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

Texture::ChannelMode image_channel_count_to_mode(const int count)
{
  switch (count)
  {
  case 1:
    return Texture::ChannelMode::R;
  case 2:
    return Texture::ChannelMode::RG;
  case 3:
    return Texture::ChannelMode::RGB;
  case 4:
    return Texture::ChannelMode::RGBA;
  default:
    break;
  }
  return Texture::ChannelMode::R;
}

}  // namespace anonymous

Texture to_texture(const Image& image, const Texture::Options& options)
{
  return Texture{image.size(), image.data(), image_channel_count_to_mode(image.channels()), options};
}

Texture load_texture(const char* filename, const Texture::Options& options)
{
  return to_texture(Image::load_from_file(filename), options);
}

}  // namespace tyl::graphics
