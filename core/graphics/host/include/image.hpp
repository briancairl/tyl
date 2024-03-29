/**
 * @copyright 2022-present Brian Cairl
 *
 * @file image.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <filesystem>

// Tyl
#include <tyl/expected.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typedef.hpp>

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
struct ImageShape : device::Shape2D
{
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
  enum class Error
  {
    kInvalidImageFile,
  };

  Image(const Image& other) = delete;

  Image(Image&& other);

  ~Image();

  /**
   * @brief Returns meta data about image
   */
  const ImageShape& shape() const noexcept { return shape_; }

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
  [[nodiscard]] static tyl::expected<Image, Error>
  load(const char* path, const ImageOptions& options = ImageOptions{}) noexcept;

  /**
   * @brief Loads image from filesystem to image data on host
   *
   * @param path  path to image file
   * @param options  image loading options
   *
   * @return image
   */
  [[nodiscard]] static tyl::expected<Image, Error>
  load(const std::filesystem::path& path, const ImageOptions& options = ImageOptions{}) noexcept
  {
    return load(path.string().c_str(), options);
  }

private:
  Image(const ImageShape& shape_data, void* const data);

  /// Meta data about an image
  ImageShape shape_;

  /// Pointer to image data
  void* data_;
};

}  // namespace tyl::graphics::host
