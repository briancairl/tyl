/**
 * @copyright 2020-present Brian Cairl
 *
 * @file image.cpp
 */

// C++ Standard Library
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

// Art
#include <tyl/graphics/image.hpp>

namespace tyl::graphics
{

Image Image::load_from_file(const char* filename, const int force_channel_count)
{
  Image im;
  int source_channels = 0;
  auto* im_ptr = stbi_load(filename, &im.rows_, &im.cols_, &source_channels, force_channel_count);
  im.channels_ = force_channel_count > 0 ? force_channel_count : source_channels;

  // Check if image point is valid
  if (im_ptr == nullptr)
  {
    std::ostringstream oss;
    oss << "Failed to load image " << filename << " : " << stbi_failure_reason();
    throw std::runtime_error{oss.str()};
  }

  static_assert(std::is_same<decltype(im_ptr), std::uint8_t*>(), "Image data not loaded as byte array");

  // Set image data to managed pointer
  im.data_.reset(im_ptr);
  return im;
}

}  // namespace tyl::graphics
