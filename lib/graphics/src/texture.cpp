/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.cpp
 */

// Art
#include <tyl/graphics/gl.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{

static GLenum channel_mode_to_gl(const Texture::Options::ChannelMode mode)
{
  switch (mode)
  {
  case Texture::Options::ChannelMode::R:
    return GL_RED;
  case Texture::Options::ChannelMode::RG:
    return GL_RG;
  case Texture::Options::ChannelMode::RGB:
    return GL_RGB;
  case Texture::Options::ChannelMode::RGBA:
    return GL_RGBA;
  default:
    break;
  }
  return GL_RED_INTEGER;
}

static GLenum wrapping_mode_to_gl(const Texture::Options::Wrapping mode)
{
  switch (mode)
  {
  case Texture::Options::Wrapping::CLAMP_TO_BORDER:
    return GL_CLAMP_TO_BORDER;
  case Texture::Options::Wrapping::REPEAT:
    return GL_REPEAT;
  default:
    break;
  }
  return GL_CLAMP_TO_BORDER;
}

static GLenum sampling_mode_to_gl(const Texture::Options::Sampling mode)
{
  switch (mode)
  {
  case Texture::Options::Sampling::LINEAR:
    return GL_LINEAR;
  case Texture::Options::Sampling::NEAREST:
    return GL_NEAREST;
  default:
    break;
  }
  return GL_NEAREST;
}

static Texture::Options::ChannelMode image_channel_count_to_mode(const int count)
{
  switch (count)
  {
  case 1:
    return Texture::Options::ChannelMode::R;
  case 2:
    return Texture::Options::ChannelMode::RG;
  case 3:
    return Texture::Options::ChannelMode::RGB;
  case 4:
    return Texture::Options::ChannelMode::RGBA;
  default:
    break;
  }
  return Texture::Options::ChannelMode::R;
}

template <GLenum GL_DATA_TYPE, typename PtrT>
static texture_id_t
create_gl_texture_2d(const long int h, const long int w, const PtrT* const data, const Texture::Options& options)
{
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_to_gl(options.u_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_to_gl(options.v_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling_mode_to_gl(options.min_sampling));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling_mode_to_gl(options.mag_sampling));

  // Original texture format
  const auto gl_original_cmode = channel_mode_to_gl(options.channel_mode);

  // Format to store texture when uploaded
  const auto gl_storage_cmode = gl_original_cmode;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, gl_original_cmode, h, w, 0, gl_storage_cmode, GL_DATA_TYPE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}

void TextureHandle::bind(const unsigned texture_index) const
{
  static const unsigned S_texture_unit_lookup[16] = {
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
  glActiveTexture(S_texture_unit_lookup[texture_index]);
  glBindTexture(GL_TEXTURE_2D, *texture_id_);
}

Texture::Texture(Texture&& other) : TextureHandle{other.texture_id_} { other.texture_id_.reset(); }

Texture& Texture::operator=(Texture&& other)
{
  new (this) Texture{std::move(other)};
  return *this;
}

Texture::Texture(const long int h, const long int w, const std::int8_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_BYTE>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const std::uint8_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_UNSIGNED_BYTE>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const std::int16_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_SHORT>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const std::uint16_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_UNSIGNED_SHORT>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const std::int32_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_INT>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const std::uint32_t* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_UNSIGNED_INT>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const float* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_FLOAT>(h, w, data, options)}
{}

Texture::Texture(const long int h, const long int w, const double* const data, const Options& options) :
    TextureHandle{create_gl_texture_2d<GL_DOUBLE>(h, w, data, options)}
{}

Texture::Texture(const Image& image) :
    Texture{image.rows(), image.cols(), image.data(), Options{image_channel_count_to_mode(image.channels())}}
{}

Texture::~Texture()
{
  if (texture_id_)
  {
    const GLuint id = *texture_id_;
    glDeleteTextures(1, &id);
  }
}

}  // namespace tyl::graphics
