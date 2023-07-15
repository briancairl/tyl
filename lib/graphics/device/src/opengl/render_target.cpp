/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target_texture.cpp
 */

// C++ Standard Library
#include <atomic>
#include <variant>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/render_target.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{

static std::atomic_flag RenderTarget__is_active{false};

expected<RenderTarget, RenderTarget::ErrorCode>
RenderTarget::create(const int height, const int width, const Options& options)
{
  if (RenderTarget__is_active.test_and_set())
  {
    return unexpected{ErrorCode::ALREADY_ACTIVE};
  }
  else if (height < 1)
  {
    return unexpected{ErrorCode::INVALID_HEIGHT};
  }
  else if (width < 1)
  {
    return unexpected{ErrorCode::INVALID_WIDTH};
  }

  if (options.enable_depth_testing)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return RenderTarget{height, width, options};
}

void RenderTarget::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer_id);
  glViewport(0, 0, height_, width_);

  if (options_.enable_depth_testing)
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  else
  {
    glClearDepth(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  glClearColor(options_.clear_color.r, options_.clear_color.g, options_.clear_color.b, options_.clear_color.a);
}

RenderTarget::RenderTarget(const int height, const int width, const Options& options) :
    height_{height}, width_{width}, options_{options}
{}

RenderTarget::RenderTarget(RenderTarget&& other) :
    height_{other.height_}, width_{other.width_}, options_{other.options_}
{
  other.height_ = 0;
  other.width_ = 0;
}

RenderTarget::~RenderTarget()
{
  if (height_ > 0)
  {
    RenderTarget__is_active.clear();
  }
}


}  // namespace tyl::graphics::device
