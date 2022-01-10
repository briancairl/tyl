/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{
namespace  // anonymous
{
GLenum channel_mode_to_gl(const TextureChannelMode mode)
{
  switch (mode)
  {
  case TextureChannelMode::R:
    return GL_RED;
  case TextureChannelMode::RG:
    return GL_RG;
  case TextureChannelMode::RGB:
    return GL_RGB;
  case TextureChannelMode::RGBA:
    return GL_RGBA;
  default:
    break;
  }
  return GL_RED_INTEGER;
}

GLenum wrapping_mode_to_gl(const TextureOptions::Wrapping mode)
{
  switch (mode)
  {
  case TextureOptions::Wrapping::CLAMP_TO_BORDER:
    return GL_CLAMP_TO_BORDER;
  case TextureOptions::Wrapping::REPEAT:
    return GL_REPEAT;
  default:
    break;
  }
  return GL_CLAMP_TO_BORDER;
}

GLenum sampling_mode_to_gl(const TextureOptions::Sampling mode)
{
  switch (mode)
  {
  case TextureOptions::Sampling::LINEAR:
    return GL_LINEAR;
  case TextureOptions::Sampling::NEAREST:
    return GL_NEAREST;
  default:
    break;
  }
  return GL_NEAREST;
}

template <GLenum GL_DATA_TYPE, typename PtrT>
texture_id_t create_gl_texture_2d(
  const unsigned h,
  const unsigned w,
  const PtrT* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options)
{
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_to_gl(options.u_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_to_gl(options.v_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling_mode_to_gl(options.min_sampling));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling_mode_to_gl(options.mag_sampling));

  // Original texture format
  const auto gl_original_cmode = channel_mode_to_gl(channel_mode);

  // Format to store texture when uploaded
  const auto gl_storage_cmode = gl_original_cmode;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, gl_original_cmode, h, w, 0, gl_storage_cmode, GL_DATA_TYPE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}
}  // namespace anonymous

Texture::Texture(const texture_id_t id) : texture_id_{id}
{
  // Debug mode check to ensure that our texture unit limit is compatible OpenGL
  TYL_ASSERT_GE(
    []() -> std::size_t {
      GLint texture_units;
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
      return texture_units;
    }(),
    texture_unit_count);
}

Texture::Texture(Texture&& other) : texture_id_{other.texture_id_} { other.texture_id_ = invalid_texture_id; }

Texture::Texture(
  const int h,
  const int w,
  const std::int8_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_BYTE>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint8_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_UNSIGNED_BYTE>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::int16_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_SHORT>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint16_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_UNSIGNED_SHORT>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::int32_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_INT>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint32_t* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_UNSIGNED_INT>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const float* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_FLOAT>(h, w, data, channel_mode, options)}
{}

Texture::Texture(
  const int h,
  const int w,
  const double* const data,
  const TextureChannelMode channel_mode,
  const TextureOptions& options) :
    Texture{create_gl_texture_2d<GL_DOUBLE>(h, w, data, channel_mode, options)}
{}

Texture::~Texture()
{
  if (Texture::valid())
  {
    glDeleteTextures(1, &texture_id_);
  }
}

Texture& Texture::operator=(Texture&& other)
{
  new (this) Texture{std::move(other)};
  return *this;
}

void Texture::bind(const unsigned texture_index) const
{
  static constexpr unsigned S_texture_unit_lookup[texture_unit_count] = {
    GL_TEXTURE0,
    GL_TEXTURE1,
    GL_TEXTURE2,
    GL_TEXTURE3,
    GL_TEXTURE4,
    GL_TEXTURE5,
    GL_TEXTURE6,
    GL_TEXTURE7,
    GL_TEXTURE8,
    GL_TEXTURE9,
    GL_TEXTURE10,
    GL_TEXTURE11,
    GL_TEXTURE12,
    GL_TEXTURE13,
    GL_TEXTURE14,
    GL_TEXTURE15,
  };

  TYL_ASSERT_NE(texture_id_, invalid_texture_id);
  glActiveTexture(S_texture_unit_lookup[texture_index]);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
}

void Texture::unbind() const { TYL_ASSERT_NE(texture_id_, invalid_texture_id); }

}  // namespace tyl::graphics::device
