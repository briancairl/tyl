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
#include <tyl/utility/expected.hpp>

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
 * @brief Data about an image
 */
struct ImageMetaData
{
  /// Height of the image, in pixels
  int height;
  /// Width of the image, in pixels
  int width;
  /// Number of image channels
  int channel_count;
};

/**
 * @brief Data about an image
 */
class Image
{
public:
  /**
   * @brief Error codes pertaining to Image
   */
  enum class ErrorCode
  {
    LOAD_FAILURE,
  };

  Image(const Image& other) = delete;

  Image(Image&& other);

  Image(const ImageMetaData& meta_data, void* const data);

  ~Image();

  /**
   * @brief Returns meta data about image
   */
  const ImageMetaData& meta() const noexcept { return meta_; }

  /**
   * @brief Creates a texture from an image
   *
   * @param image  path to image file
   * @param options  image loading options
   *
   * @return image
   */
  device::Texture texture(const device::TextureOptions& options = {}) const noexcept;

  /**
   * @brief Loads image from filesystem to image data on host
   *
   * @param path  path to image file
   * @param options  image loading options
   *
   * @return image
   */
  [[nodiscard]] static tyl::expected<Image, ErrorCode>
  load(const char* path, const ImageOptions& options = ImageOptions{}) noexcept;

private:
  /// Meta data about an image
  ImageMetaData meta_;

  /// Pointer to image data
  void* data_;
};

}  // namespace tyl::graphics::host
