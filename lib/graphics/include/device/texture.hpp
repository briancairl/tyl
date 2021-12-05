/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <optional>
#include <type_traits>

// Tyl
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

enum class TextureChannelMode
{
  R,  //< Red (1-channel)
  RG,  //< Red-green (2-channel)
  RGB,  //< Red-green-blue (3-channel)
  RGBA,  //< Red-green-blue-alpha (4-channel)
};

struct TextureOptions
{
  enum class Wrapping
  {
    CLAMP_TO_BORDER,
    REPEAT
  };

  enum class Sampling
  {
    LINEAR,
    NEAREST
  };

  Wrapping u_wrapping = Wrapping::CLAMP_TO_BORDER;

  Wrapping v_wrapping = Wrapping::CLAMP_TO_BORDER;

  Sampling min_sampling = Sampling::NEAREST;

  Sampling mag_sampling = Sampling::NEAREST;

  TextureOptions() = default;
};

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
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const double* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int8_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint8_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int16_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint16_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::int32_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const int h,
    const int w,
    const std::uint32_t* const data,
    const TextureChannelMode mode = TextureChannelMode::R,
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
  inline texture_id_t get_id() const { return texture_id_.value(); };

  /**
   * @brief Checks if texture is currently ready for use
   */
  inline bool valid() const { return is_bound_ and texture_id_; }

  /**
   * @brief Checks if texture is currently ready for use
   */
  inline operator bool() const { return Texture::valid(); }

private:
  Texture(const Texture&) = default;

  explicit Texture(const texture_id_t id) : texture_id_{id}, is_bound_{false} {};

  /// Device texture ID
  std::optional<texture_id_t> texture_id_;

  /// Tracks if texture is bound to device
  mutable bool is_bound_;
};

}  // namespace tyl::graphics::device
