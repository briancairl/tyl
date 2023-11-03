/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target_texture.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/expected.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

/**
 * @brief RenderTargetTexture creation options
 */
struct RenderTargetTextureOptions
{
  /// Enables depth-testing buffer
  bool enable_depth_testing = false;
  /// Specifies number of channels for color buffer
  TextureChannels texture_channels = TextureChannels::RGBA;
  /// Texture element data type
  TypeCode texture_depth = TypeCode::UInt8;
};

/**
 * @brief Intermediate memory buffer (texture) for rendering
 *
 * Can be used in place of RenderTargetBuffer (back/screen buffer) for rendering to an intermediate
 * memory buffer.
 */
class RenderTargetTexture
{
public:
  using Options = RenderTargetTextureOptions;

  /**
   * @brief Possible RenderTargetTexture creation errors
   */
  enum class Error
  {
    kDeviceConfigurationFailure,
    kTextureCreationFailure,
    kInvalidTextureHeight,
    kInvalidTextureWidth,
  };

  [[nodiscard]] static expected<RenderTargetTexture, Error> create(const Shape2D& shape, const Options& options = {});

  template <typename DrawToTextureT> void draw_to(DrawToTextureT draw_to_texture)
  {
    RenderTargetTexture::bind();
    draw_to_texture(target_texture_.shape());
    RenderTargetTexture::unbind();
  }

  const Texture& texture() const { return target_texture_; }

  RenderTargetTexture(RenderTargetTexture&& other);

  ~RenderTargetTexture();

private:
  void bind() const;
  void unbind() const;

  RenderTargetTexture(const RenderTargetTexture&) = delete;

  RenderTargetTexture(
    Texture&& texture,
    const frame_buffer_id_t frame_buffer_id,
    const std::optional<frame_buffer_id_t> depth_buffer_id);

  /// Target texture
  Texture target_texture_;

  /// Frame buffer ID
  frame_buffer_id_t frame_buffer_id_;

  /// Depth buffer ID
  std::optional<frame_buffer_id_t> depth_buffer_id_;
};

}  // namespace tyl::graphics::device
