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
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typecode.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

enum class TextureChannels
{
  R,  //< Red (1-channel)
  RG,  //< Red-green (2-channel)
  RGB,  //< Red-green-blue (3-channel)
  RGBA,  //< Red-green-blue-alpha (4-channel)
};

/**
 * @brief Non-owning texture data view
 */
struct TextureView
{
public:
  /// Pointer to data
  inline auto* data() { return data_; }

  /// Pointer to data
  inline const auto* data() const { return data_; }

  /// Size of view, in bytes
  constexpr std::size_t size() const { return size_; }

  /// Height of 2D texture, in pixels
  constexpr int height() const { return height_; }

  /// Width of 2D texture, in pixels
  constexpr int width() const { return width_; }

  /// Texture element type code
  constexpr TypeCode type() const { return typecode_; }

  /// Number of channels per texture element
  constexpr TextureChannels channels() const { return channels_; }

  TextureView(void* const data, const int h, const int w, const TypeCode typecode, const TextureChannels channels);

  template <typename T> T* element(int i, int j) { return reinterpret_cast<T*>(data()) + i * width_ + j; }

  template <typename T> const T* element(int i, int j) const
  {
    return reinterpret_cast<const T*>(data()) + i * width_ + j;
  }

protected:
  TextureView() = default;

  void* data_;
  std::size_t size_;
  int height_;
  int width_;
  TypeCode typecode_;
  TextureChannels channels_;

  friend class Texture;
  friend class TextureHandle;
};

/**
 * @brief Texture data, downloaded to host
 */
struct TextureHost : public TextureView
{
public:
  TextureHost(const TextureHandle& texture);

  TextureHost(
    std::unique_ptr<std::uint8_t[]>&& data,
    const int h,
    const int w,
    const TypeCode typecode,
    const TextureChannels channels);

  inline bool valid() const { return static_cast<bool>(owned_); }

private:
  using TextureView::TextureView;

  TextureHost() = default;

  std::unique_ptr<std::uint8_t[]> owned_;

  friend class Texture;
  friend class TextureHandle;
};

struct TextureOptions
{
  enum class Wrapping : std::uint8_t
  {
    CLAMP_TO_BORDER,
    REPEAT
  };

  enum class Sampling : std::uint8_t
  {
    LINEAR,
    NEAREST
  };

  Wrapping u_wrapping = Wrapping::CLAMP_TO_BORDER;
  Wrapping v_wrapping = Wrapping::CLAMP_TO_BORDER;

  Sampling min_sampling = Sampling::NEAREST;
  Sampling mag_sampling = Sampling::NEAREST;

  struct
  {
    std::uint8_t unpack_alignment : 1;
    std::uint8_t generate_mip_map : 1;
  } flags = {1, 1};

  TextureOptions() = default;
};

/**
 * @brief Weak reference to a Texture
 */
class TextureHandle
{
public:
  TextureHandle(TextureHandle&& other);
  TextureHandle(const TextureHandle& other) = default;

  ~TextureHandle() = default;

  TextureHandle& operator=(TextureHandle&&);
  TextureHandle& operator=(const TextureHandle&) = default;

  /**
   * @brief Uploads new texture
   */
  void upload(const TextureView& texture_data, const TextureOptions& texture_options) const;

  /**
   * @brief Downloads texture to host
   */
  [[nodiscard]] TextureHost download() const;

  /**
   * @brief Downloads texture to host
   */
  [[nodiscard]] TextureHost download(TextureOptions& options) const;

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
  [[nodiscard]] constexpr bool valid() const { return typecode_ != TypeCode::Invalid; }

  /**
   * @brief Checks if texture is currently ready for use
   */
  [[nodiscard]] constexpr operator bool() const { return TextureHandle::valid(); }

  /**
   * @brief Binds texture to a working texture unit
   */
  void bind(const index_t texture_index) const;

  /**
   * @brief Unbinds texture from a working texture unit
   */
  void unbind() const;

protected:
  explicit TextureHandle(const texture_id_t id, const TypeCode typecode);

  /// Device texture ID
  texture_id_t texture_id_;

  /// Device texture data typecode
  TypeCode typecode_;
};

/**
 * @brief RAII wrapper around a texture resource
 *
 *        Creates and destroys texture through graphics API
 *
 * @warning Do not pass around Texture, use TextureHandle
 */
class Texture : public TextureHandle
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

  explicit Texture(const TextureView& texture_data, const TextureOptions& texture_options = TextureOptions{});

  ~Texture();

  Texture& operator=(Texture&&);

private:
  Texture(const Texture&) = default;
};

}  // namespace tyl::graphics::device
