/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target_texture.cpp
 */

// C++ Standard Library
#include <mutex>
#include <thread>
#include <unordered_set>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/render_target.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{
namespace
{

std::mutex RenderTarget_active_context_mutex;
std::unordered_set<void*> RenderTarget_active_contexts;

}  // namespace

expected<RenderTarget, RenderTarget::ErrorCode>
RenderTarget::create(void* const context, const Shape2D& shape, const Options& options)
{
  if (context == nullptr)
  {
    return unexpected<ErrorCode>{ErrorCode::CONTEXT_INVALID};
  }

  std::lock_guard lock{RenderTarget_active_context_mutex};
  if (RenderTarget_active_contexts.count(context) > 0)
  {
    return unexpected<ErrorCode>{ErrorCode::CONTEXT_IN_USE};
  }
  else if (shape.height < 1)
  {
    return unexpected<ErrorCode>{ErrorCode::INVALID_HEIGHT};
  }
  else if (shape.width < 1)
  {
    return unexpected<ErrorCode>{ErrorCode::INVALID_WIDTH};
  }

  if (options.enable_depth_testing)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return RenderTarget{context, shape, options};
}

void RenderTarget::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer_id);
  glViewport(0, 0, shape_.height, shape_.width);

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

RenderTarget::RenderTarget(void* const context, const Shape2D& shape, const Options& options) :
    context_{context}, shape_{shape}, options_{options}
{}

RenderTarget::RenderTarget(RenderTarget&& other) :
    context_{other.context_}, shape_{other.shape_}, options_{other.options_}
{
  other.context_ = nullptr;
}

RenderTarget::~RenderTarget()
{
  if (context_ == nullptr)
  {
    return;
  }
  else
  {
    std::lock_guard lock{RenderTarget_active_context_mutex};
    RenderTarget_active_contexts.erase(context_);
  }
}


}  // namespace tyl::graphics::device
