/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

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

  /// Shape of 2D texture, in pixels
  constexpr const Shape2D& shape() const { return shape_; }

  /// Texture element type code
  constexpr TypeCode type() const { return typecode_; }

  /// Number of channels per texture element
  constexpr TextureChannels channels() const { return channels_; }

  TextureView(void* const data, const Shape2D& shape, const TypeCode typecode, const TextureChannels channels);

  TextureView(float* const data, const Shape2D& shape, const TextureChannels channels);

  TextureView(std::uint8_t* const data, const Shape2D& shape, const TextureChannels channels);

  TextureView(std::uint16_t* const data, const Shape2D& shape, const TextureChannels channels);

  TextureView(std::uint32_t* const data, const Shape2D& shape, const TextureChannels channels);

  template <typename T> T* element(int i, int j) { return reinterpret_cast<T*>(data()) + i * shape_.width + j; }

  template <typename T> const T* element(int i, int j) const
  {
    return reinterpret_cast<const T*>(data()) + i * shape_.width + j;
  }

  template <typename T> const T* begin() const { return reinterpret_cast<const T*>(data()); }

  template <typename T> const T* end() const
  {
    return reinterpret_cast<const T*>(data()) + (shape_.width * shape_.height);
  }

  inline bool valid() const { return data_ != nullptr; }

protected:
  TextureView() = default;

  void* data_;
  std::size_t size_;
  Shape2D shape_;
  TypeCode typecode_;
  TextureChannels channels_;

  friend class Texture;
  friend class TextureHandle;
};

/**
 * @brief Texture data, downloaded to host
 */
class TextureHost : public TextureView
{
public:
  TextureHost(const TextureHandle& texture);

  ~TextureHost();

private:
  using TextureView::TextureView;

  TextureHost() = default;
  TextureHost(void* const data, const Shape2D& shape, const TypeCode typecode, const TextureChannels channels);

  friend class Texture;
  friend class TextureHandle;
};

struct TextureOptions
{
  enum class Wrapping : std::uint8_t
  {
    kClampToBorder,
    kRepeat
  };

  enum class Sampling : std::uint8_t
  {
    kLinear,
    kNearest
  };

  Wrapping u_wrapping = Wrapping::kClampToBorder;
  Wrapping v_wrapping = Wrapping::kClampToBorder;

  Sampling min_sampling = Sampling::kNearest;
  Sampling mag_sampling = Sampling::kNearest;

  struct
  {
    std::uint8_t unpack_alignment : 1;
    std::uint8_t generate_mip_map : 1;
  } flags = {1, 1};
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
  void upload(const TextureView& texture_data, const TextureOptions& texture_options = TextureOptions{}) const;

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
   * @brief Sets texture to an active state
   */
  void bind() const;

  /**
   * @brief Sets texture to an active state and binds to a texture unit
   */
  void bind(const index_t texture_index) const;

  /**
   * @brief Unbinds texture from a working texture unit
   */
  void unbind() const;

  /**
   * @brief Returns texture height
   */
  const Shape2D& shape() const { return shape_; }

protected:
  TextureHandle(const texture_id_t id, const TypeCode typecode, const Shape2D& shape);

  /// Device texture ID
  texture_id_t texture_id_;

  /// Device texture data typecode
  TypeCode typecode_;

  /// Texture shape
  Shape2D shape_;
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
    const Shape2D& shape,
    const TypeCode type,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const float* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const double* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::int8_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::uint8_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::int16_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::uint16_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::int32_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});
  Texture(
    const Shape2D& shape,
    const std::uint32_t* const data,
    const TextureChannels mode = TextureChannels::R,
    const TextureOptions& options = TextureOptions{});

  explicit Texture(const TextureView& texture_data, const TextureOptions& texture_options);

  ~Texture();

  Texture& operator=(Texture&&);

private:
  Texture(const Texture&) = default;
};

}  // namespace tyl::graphics::device
