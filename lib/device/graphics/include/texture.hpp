/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/device/graphics/constants.hpp>
#include <tyl/device/graphics/fwd.hpp>
#include <tyl/device/graphics/texture_options.hpp>
#include <tyl/device/graphics/typedef.hpp>

namespace tyl::device::graphics
{

/**
 * @brief RAII wrapper around a texture resource
 *
 *        Creates and destroys texture through graphics API
 *
 * @warning Do not pass around Texture, use TextureHandle
 */
class Texture
{
public:
  Texture(Texture&& other);
  Texture(
    const int h,
    const int w,
    const float* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const double* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int8_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint8_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int16_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint16_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int32_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint32_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});

  ~Texture();

  Texture& operator=(Texture&&);

  /**
   * @brief Binds texture to a working texture unit
   */
  void bind(const unsigned texture_index) const;

  /**
   * @brief Unbinds texture from a working texture unit
   */
  void unbind() const;

  /**
   * @brief Returns unique ID associated with loaded texture
   */
  inline texture_id_t get_id() const { return texture_id_; };

  /**
   * @brief Checks if texture is currently ready for use
   */
  inline bool valid() const { return texture_id_ != invalid_texture_id; }

  /**
   * @brief Checks if texture is currently ready for use
   */
  inline operator bool() const { return Texture::valid(); }

private:
  Texture(const Texture&) = default;

  explicit Texture(const texture_id_t id);

  /// Device texture ID
  texture_id_t texture_id_;
};

}  // namespace tyl::device::graphics
