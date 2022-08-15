/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.cpp
 */

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/gl.inl>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/texture.hpp>

namespace tyl::graphics::device
{
namespace  // anonymous
{

GLenum channels_to_gl(const TextureChannels mode)
{
  switch (mode)
  {
  case TextureChannels::R:
    return GL_RED;
  case TextureChannels::RG:
    return GL_RG;
  case TextureChannels::RGB:
    return GL_RGB;
  case TextureChannels::RGBA:
    return GL_RGBA;
  default:
    break;
  }
  return GL_RED_INTEGER;
}

std::size_t channels_to_count(const TextureChannels mode)
{
  switch (mode)
  {
  case TextureChannels::R:
    return 1;
  case TextureChannels::RG:
    return 2;
  case TextureChannels::RGB:
    return 3;
  case TextureChannels::RGBA:
    return 4;
  default:
    break;
  }
  return 0;
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

TextureChannels channels_from_gl(const GLenum mode)
{
  switch (mode)
  {
  case GL_RED:
    return TextureChannels::R;
  case GL_RG:
    return TextureChannels::RG;
  case GL_RGB:
    return TextureChannels::RGB;
  case GL_RGBA:
    return TextureChannels::RGBA;
  default:
    break;
  }
  return TextureChannels::R;
}

TextureOptions::Wrapping wrapping_mode_from_gl(const GLenum mode)
{
  switch (mode)
  {
  case GL_CLAMP_TO_BORDER:
    return TextureOptions::Wrapping::CLAMP_TO_BORDER;
  case GL_REPEAT:
    return TextureOptions::Wrapping::REPEAT;
  default:
    break;
  }
  return TextureOptions::Wrapping::CLAMP_TO_BORDER;
}

TextureOptions::Sampling sampling_mode_from_gl(const GLenum mode)
{
  switch (mode)
  {
  case GL_LINEAR:
    return TextureOptions::Sampling::LINEAR;
  case GL_NEAREST:
    return TextureOptions::Sampling::NEAREST;
  default:
    break;
  }
  return TextureOptions::Sampling::NEAREST;
}

template <typename PtrT>
texture_id_t create_gl_texture_2d(
  const int h,
  const int w,
  const PtrT* const data,
  const TextureChannels channels,
  const TextureOptions& options,
  const TypeCode type = typecode<std::remove_pointer_t<PtrT>>())
{
  TYL_ASSERT_GT(h, 0);
  TYL_ASSERT_GT(w, 0);
  TYL_ASSERT_NON_NULL(data);

  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_to_gl(options.u_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_to_gl(options.v_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling_mode_to_gl(options.min_sampling));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling_mode_to_gl(options.mag_sampling));

  // Original texture format
  const auto gl_original_cmode = channels_to_gl(channels);

  // Format to store texture when uploaded
  const auto gl_storage_cmode = gl_original_cmode;

  if (options.flags.unpack_alignment)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_original_cmode,
    h,
    w,
    0,
    gl_storage_cmode,
    to_gl_typecode(type),
    reinterpret_cast<const void*>(data));

  if (options.flags.generate_mip_map)
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}

void download_gl_texture_options(TextureOptions& options)
{
  {
    GLint v;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &v);
    options.u_wrapping = wrapping_mode_from_gl(v);
  }

  {
    GLint v;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &v);
    options.v_wrapping = wrapping_mode_from_gl(v);
  }

  {
    GLint v;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &v);
    options.min_sampling = sampling_mode_from_gl(v);
  }

  {
    GLint v;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &v);
    options.mag_sampling = sampling_mode_from_gl(v);
  }
}

void download_gl_texture_image(
  std::unique_ptr<std::uint8_t[]>& data,
  int& h,
  int& w,
  TextureChannels& channels,
  const TypeCode& typecode)
{
  static constexpr GLint MIP_LEVEL = 0;

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_WIDTH, &v);
    h = v;
  }

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_HEIGHT, &v);
    w = v;
  }

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_INTERNAL_FORMAT, &v);
    channels = channels_from_gl(v);
  }

  {
    const std::size_t bytes = (h * w) * byte_count(typecode) * channels_to_count(channels);
    data.reset(new std::uint8_t[bytes]);
  }


  glGetTexImage(
    GL_TEXTURE_2D, MIP_LEVEL, channels_to_gl(channels), to_gl_typecode(typecode), reinterpret_cast<void*>(data.get()));
}

}  // namespace anonymous

TextureHost::TextureHost(const TextureHandle& texture) : TextureHost{texture.download()} {}

TextureHost::TextureHost(
  std::unique_ptr<std::uint8_t[]>&& data,
  const int h,
  const int w,
  const TypeCode typecode,
  const TextureChannels channels) :
    data_{std::move(data)}, height_{h}, width_{w}, typecode_{typecode}, channels_{channels}
{}

TextureHandle::TextureHandle(TextureHandle&& other) : TextureHandle{other.texture_id_, other.typecode_}
{
  other.typecode_ = TypeCode::Invalid;
}

TextureHandle::TextureHandle(const texture_id_t id, const TypeCode typecode) : texture_id_{id}, typecode_{typecode} {}

TextureHost TextureHandle::download() const
{
  TYL_ASSERT_NE(typecode_, TypeCode::Invalid);

  TextureHost texture_host;

  glBindTexture(GL_TEXTURE_2D, texture_id_);

  download_gl_texture_image(
    texture_host.data_, texture_host.height_, texture_host.width_, texture_host.channels_, typecode_);

  texture_host.typecode_ = typecode_;

  return texture_host;
}

TextureHost TextureHandle::download(TextureOptions& options) const
{
  TYL_ASSERT_NE(typecode_, TypeCode::Invalid);

  TextureHost texture_host;

  glBindTexture(GL_TEXTURE_2D, texture_id_);

  download_gl_texture_options(options);

  download_gl_texture_image(
    texture_host.data_, texture_host.height_, texture_host.width_, texture_host.channels_, typecode_);

  texture_host.typecode_ = typecode_;

  return texture_host;
}

TextureHandle& TextureHandle::operator=(TextureHandle&& other)
{
  new (this) TextureHandle{std::move(other)};
  return *this;
}

void TextureHandle::bind(const index_t texture_index) const
{
  TYL_ASSERT_NE(typecode_, TypeCode::Invalid);

  // Debug mode check to ensure that our texture unit limit is compatible OpenGL
  TYL_ASSERT_GE(
    []() -> std::size_t {
      GLint texture_units;
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
      return texture_units;
    }(),
    texture_unit_count);

  static constexpr GLenum S_texture_unit_lookup[texture_unit_count] = {
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
  glBindTexture(GL_TEXTURE_2D, texture_id_);
}

void TextureHandle::unbind() const { TYL_ASSERT_NE(typecode_, TypeCode::Invalid); }

Texture::Texture(Texture&& other) : TextureHandle{std::move(static_cast<TextureHandle&&>(other))} {}

Texture::Texture(
  const int h,
  const int w,
  const std::int8_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::int8_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint8_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::uint8_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::int16_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::int16_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint16_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::uint16_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::int32_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::int32_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const std::uint32_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<std::uint32_t>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const float* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<float>()}
{}

Texture::Texture(
  const int h,
  const int w,
  const double* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(h, w, data, channels, options), typecode<double>()}
{}

Texture::Texture(const TextureHost& texture_data, const TextureOptions& texture_options) :
    TextureHandle{
      create_gl_texture_2d<std::uint8_t>(
        texture_data.height_,
        texture_data.width_,
        texture_data.data_.get(),
        texture_data.channels_,
        texture_options,
        texture_data.typecode_),
      texture_data.typecode_}
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


}  // namespace tyl::graphics::device
