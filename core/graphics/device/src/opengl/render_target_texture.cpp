/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_target_texture.cpp
 */

// C++ Standard Library
#include <variant>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/render_target_texture.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{

expected<RenderTargetTexture, RenderTargetTexture::ErrorCode>
RenderTargetTexture::create(const Shape2D& shape, const Options& options)
{
  if (shape.height < 1)
  {
    return unexpected<ErrorCode>{ErrorCode::INVALID_TEXTURE_HEIGHT};
  }
  else if (shape.width < 1)
  {
    return unexpected<ErrorCode>{ErrorCode::INVALID_TEXTURE_WIDTH};
  }

  Texture texture{shape, options.texture_depth, options.texture_channels};
  if (texture.get_id() == invalid_texture_id)
  {
    return unexpected<ErrorCode>{ErrorCode::TEXTURE_CREATION_FAILURE};
  }

  // Create a new frame buffer
  frame_buffer_id_t frame_buffer_id = 0;
  glGenFramebuffers(1, &frame_buffer_id);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.get_id(), 0);

  // Set the list of draw buffers.
  {
    enum_t buffer_spec[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffer_spec);
  }

  // Create a depth testing buffer
  std::optional<frame_buffer_id_t> depth_buffer_id{std::nullopt};
  if (options.enable_depth_testing)
  {
    depth_buffer_id.emplace();
    glGenRenderbuffers(1, &(*depth_buffer_id));
    glBindRenderbuffer(GL_RENDERBUFFER, (*depth_buffer_id));
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture.shape().width, texture.shape().height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (*depth_buffer_id));
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
  {
    return RenderTargetTexture{std::move(texture), frame_buffer_id, depth_buffer_id};
  }
  return unexpected<ErrorCode>{ErrorCode::DEVICE_CONFIGURATION_FAILURE};
}

void RenderTargetTexture::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
  glViewport(0, 0, target_texture_.shape().height, target_texture_.shape().width);

  if (depth_buffer_id_.has_value())
  {
    glClearDepth(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

void RenderTargetTexture::unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer_id); }

RenderTargetTexture::RenderTargetTexture(
  Texture&& texture,
  const frame_buffer_id_t frame_buffer_id,
  const std::optional<frame_buffer_id_t> depth_buffer_id) :
    target_texture_{std::move(texture)}, frame_buffer_id_{frame_buffer_id}, depth_buffer_id_{depth_buffer_id}
{}

RenderTargetTexture::RenderTargetTexture(RenderTargetTexture&& other) :
    target_texture_{std::move(other.target_texture_)},
    frame_buffer_id_{other.frame_buffer_id_},
    depth_buffer_id_{other.depth_buffer_id_}
{
  other.frame_buffer_id_ = default_frame_buffer_id;
  other.depth_buffer_id_ = std::nullopt;
}

RenderTargetTexture::~RenderTargetTexture()
{
  if (frame_buffer_id_ == default_frame_buffer_id)
  {
    return;
  }

  glDeleteFramebuffers(1, &frame_buffer_id_);

  if (depth_buffer_id_ == std::nullopt)
  {
    return;
  }

  glDeleteRenderbuffers(1, &(*depth_buffer_id_));
}


}  // namespace tyl::graphics::device
