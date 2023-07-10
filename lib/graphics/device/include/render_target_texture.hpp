/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target_texture.hpp
 */
#pragma once

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/device/typedef.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::graphics::device
{


/**
 * @brief RenderTargetTexture creation options
 */
struct RenderTargetTextureOptions
{
  bool enable_depth_testing = false;
  bool enable_alpha = true;
  TypeCode texture_depth = TypeCode::UInt8;
};

/**
 * @brief Render target texture
 */
class RenderTargetTexture
{
public:
  using Options = RenderTargetTextureOptions;

  /**
   * @brief Possible RenderTargetTexture creation errors
   */
  enum class ErrorCode
  {
    SETUP_FAILURE,
    INVALID_TEXTURE,
    INVALID_MAX_HEIGHT,
    INVALID_MAX_WIDTH,
  };

  static expected<RenderTargetTexture, ErrorCode>
  create(const int height, const int width, const Options& options = {});

  template <typename DrawToTextureT> void draw_to(DrawToTextureT draw_to_texture)
  {
    RenderTargetTexture::bind();
    draw_to_texture(target_texture_.height(), target_texture_.width());
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

  Texture target_texture_;
  frame_buffer_id_t frame_buffer_id_;
  std::optional<frame_buffer_id_t> depth_buffer_id_;
};

}  // namespace tyl::graphics::device
