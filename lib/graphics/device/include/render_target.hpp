/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target.hpp
 */
#pragma once

// Tyl
#include <tyl/graphics/device/typedef.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::graphics::device
{

/**
 * @brief RenderTarget creation options
 */
struct RenderTargetOptions
{
  /// Enables depth-testing buffer
  bool enable_depth_testing = false;
  /// Color used to clear buffer before rendering
  Color clear_color = {};
};

/**
 * @brief Manages the default screen (back) buffer
 */
class RenderTarget
{
public:
  using Options = RenderTargetOptions;

  /**
   * @brief Possible RenderTarget creation errors
   */
  enum class ErrorCode
  {
    ALREADY_ACTIVE,
    DEVICE_CONFIGURATION_FAILURE,
    INVALID_HEIGHT,
    INVALID_WIDTH,
  };

  [[nodiscard]] static expected<RenderTarget, ErrorCode> create(const Shape2D& shape, const Options& options = {});

  template <typename ResizeBufferFnT, typename DrawToBufferT>
  void draw_to(ResizeBufferFnT resize_buffer, DrawToBufferT draw_to_buffer)
  {
    RenderTarget::bind();
    resize_buffer(shape_);
    draw_to_buffer(static_cast<const Shape2D&>(shape_));
  }

  template <typename DrawToBufferT> void draw_to(DrawToBufferT&& draw_to_buffer)
  {
    draw_to([]([[maybe_unused]] const Shape2D& shape) {}, std::forward<DrawToBufferT>(draw_to_buffer));
  }

  RenderTarget(RenderTarget&& other);

  ~RenderTarget();

private:
  void bind() const;

  RenderTarget(const RenderTarget&) = delete;

  RenderTarget(const Shape2D& shape, const Options& options);

  /// Current back buffer viewport extents
  Shape2D shape_;
  /// Back buffer options
  Options options_;
};

}  // namespace tyl::graphics::device
