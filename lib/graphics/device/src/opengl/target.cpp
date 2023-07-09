/**
 * @copyright 2023-present Brian Cairl
 *
 * @file target.cpp
 */

// C++ Standard Library
#include <variant>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/target.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{


expected<Target, Target::ErrorCode>
Target::create(const int max_height, const int max_width, [[maybe_unused]] const Options& options)
{
  if (max_height < 1)
  {
    return unexpected{ErrorCode::INVALID_MAX_HEIGHT};
  }
  else if (max_width < 1)
  {
    return unexpected{ErrorCode::INVALID_MAX_WIDTH};
  }
  else
  {
    return Target{max_height, max_width, default_target_id, std::nullopt};
  }
}

expected<Target, Target::ErrorCode> Target::create(const TextureHandle& render_texture, const Options& options)
{
  if (render_texture.get_id() == invalid_texture_id)
  {
    return unexpected{ErrorCode::INVALID_RENDER_TEXTURE};
  }

  // Create a new frame buffer
  target_id_t target_id = 0;
  glGenFramebuffers(1, &target_id);
  glBindFramebuffer(GL_FRAMEBUFFER, target_id);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture.get_id(), 0);

  // Set the list of draw buffers.
  {
    enum_t buffer_spec[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffer_spec);
  }

  // Create a depth testing buffer
  std::optional<target_id_t> depth_target_id{std::nullopt};
  if (options.enable_depth_testing)
  {
    depth_target_id.emplace();
    glGenRenderbuffers(1, &(*depth_target_id));
    glBindRenderbuffer(GL_RENDERBUFFER, (*depth_target_id));
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_texture.width(), render_texture.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (*depth_target_id));
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
  {
    return Target{render_texture.height(), render_texture.width(), target_id, depth_target_id};
  }
  return unexpected{ErrorCode::INVALID_TARGET};
}

void Target::bind(const int height, const int width) const
{
  glBindFramebuffer(GL_FRAMEBUFFER, target_id_);
  glViewport(0, 0, height, width);
}

void Target::bind() const { Target::bind(max_height_, max_width_); }

Target::Target(
  const int max_height,
  const int max_width,
  const target_id_t target_id,
  const std::optional<target_id_t> depth_target_id) :
    max_height_{max_height}, max_width_{max_width}, target_id_{target_id}, depth_target_id_{depth_target_id}
{}

Target::Target(Target&& other) :
    max_height_{other.max_height_},
    max_width_{other.max_width_},
    target_id_{other.target_id_},
    depth_target_id_{other.depth_target_id_}
{
  other.target_id_ = default_target_id;
  other.depth_target_id_ = std::nullopt;
}

Target::~Target()
{
  if (target_id_ == default_target_id)
  {
    return;
  }

  glDeleteFramebuffers(1, &target_id_);

  if (depth_target_id_ == std::nullopt)
  {
    return;
  }

  glDeleteRenderbuffers(1, &(*depth_target_id_));
}


}  // namespace tyl::graphics::device
