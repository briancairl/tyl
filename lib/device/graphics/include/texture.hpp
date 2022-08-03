/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <memory>

// Tyl
#include <tyl/device/graphics/constants.hpp>
#include <tyl/device/graphics/fwd.hpp>
#include <tyl/device/graphics/texture_options.hpp>
#include <tyl/device/graphics/typecode.hpp>
#include <tyl/device/graphics/typedef.hpp>

namespace tyl::device::graphics
{

/**
 * @brief Texture data, downloaded to host
 */
struct TextureHost
{
public:
  inline auto* data() { return data_.get(); }
  inline const auto* data() const { return data_.get(); }
  constexpr std::size_t size() const { return height_ * width_; }
  constexpr TypeCode type() const { return typecode_; }
  constexpr TextureChannels channels() const { return channels_; }
  constexpr const TextureOptions& options() const { return options_; }

  inline bool valid() const { return static_cast<bool>(data_); }

  TextureHost(const Texture& texture);

  TextureHost(
    std::unique_ptr<std::uint8_t>&& data,
    const int h,
    const int w,
    const TypeCode typecode,
    const TextureChannels channels,
    const TextureOptions& options);

private:
  TextureHost() = default;

  std::unique_ptr<std::uint8_t> data_;
  int height_;
  int width_;
  TypeCode typecode_;
  TextureChannels channels_;
  TextureOptions options_;

  friend class Texture;
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

  explicit Texture(const TextureHost& texture_data);

  ~Texture();

  Texture& operator=(Texture&&);

  /**
   * @brief Downloads texture to host
   */
  [[nodiscard]] TextureHost download() const;

  /**
   * @brief Binds texture to a working texture unit
   */
  void bind(const index_t texture_index) const;

  /**
   * @brief Unbinds texture from a working texture unit
   */
  void unbind() const;

  /**
   * @brief Returns unique ID associated with loaded texture
   */
  [[nodiscard]] constexpr texture_id_t get_id() const { return texture_id_; };

  /**
   * @brief Returns texture type code
   */
  [[nodiscard]] constexpr TypeCode type() const { return typecode_; };

  /**
   * @brief Checks if texture is currently ready for use
   */
  [[nodiscard]] constexpr bool valid() const { return texture_id_ != invalid_texture_id; }

  /**
   * @brief Checks if texture is currently ready for use
   */
  [[nodiscard]] constexpr operator bool() const { return Texture::valid(); }

private:
  Texture(const Texture&) = default;

  explicit Texture(const texture_id_t id, const TypeCode typecode);

  /// Device texture ID
  texture_id_t texture_id_;

  /// Device texture data typecode
  TypeCode typecode_;
};

}  // namespace tyl::device::graphics
