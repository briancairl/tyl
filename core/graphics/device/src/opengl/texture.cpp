/**
 * @copyright 2020-present Brian Cairl
 *
 * @file texture.cpp
 */

// C++ Standard Library
#include <tuple>

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
  case TextureOptions::Wrapping::kClampToBorder:
    return GL_CLAMP_TO_BORDER;
  case TextureOptions::Wrapping::kRepeat:
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
  case TextureOptions::Sampling::kLinear:
    return GL_LINEAR;
  case TextureOptions::Sampling::kNearest:
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
    return TextureOptions::Wrapping::kClampToBorder;
  case GL_REPEAT:
    return TextureOptions::Wrapping::kRepeat;
  default:
    break;
  }
  return TextureOptions::Wrapping::kClampToBorder;
}

TextureOptions::Sampling sampling_mode_from_gl(const GLenum mode)
{
  switch (mode)
  {
  case GL_LINEAR:
    return TextureOptions::Sampling::kLinear;
  case GL_NEAREST:
    return TextureOptions::Sampling::kNearest;
  default:
    break;
  }
  return TextureOptions::Sampling::kNearest;
}

texture_id_t gen_gl_texture_2d(const TextureOptions& options)
{
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_to_gl(options.u_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_to_gl(options.v_wrapping));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling_mode_to_gl(options.min_sampling));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling_mode_to_gl(options.mag_sampling));

  if (options.flags.unpack_alignment)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }
  return id;
}

void upload_gl_texture_2d(
  const Shape2D& shape,
  const void* const data,
  const TextureChannels channels,
  const TextureOptions& options,
  const TypeCode type)
{
  TYL_ASSERT_GT(shape.height, 0);
  TYL_ASSERT_GT(shape.width, 0);

  // Original texture format
  const auto gl_original_cmode = channels_to_gl(channels);

  // Format to store texture when uploaded
  const auto gl_storage_cmode = gl_original_cmode;

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_original_cmode,
    shape.height,
    shape.width,
    0,
    gl_storage_cmode,
    to_gl_typecode(type),
    reinterpret_cast<const void*>(data));

  if (options.flags.generate_mip_map)
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
}

template <typename PtrT>
texture_id_t create_gl_texture_2d(
  const Shape2D& shape,
  const PtrT* const data,
  const TextureChannels channels,
  const TextureOptions& options,
  const TypeCode type = typecode<std::remove_pointer_t<PtrT>>())
{
  const auto id = gen_gl_texture_2d(options);

  TYL_ASSERT_NON_NULL(data);

  upload_gl_texture_2d(shape, data, channels, options, type);

  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}

texture_id_t create_gl_empty_texture_2d(
  const Shape2D& shape,
  const TextureChannels channels,
  const TextureOptions& options,
  const TypeCode type)
{
  const auto id = gen_gl_texture_2d(options);

  upload_gl_texture_2d(shape, nullptr, channels, options, type);

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

std::tuple<void*, std::size_t>
download_gl_texture_image(Shape2D& shape, TextureChannels& channels, const TypeCode& typecode)
{
  static constexpr GLint MIP_LEVEL = 0;

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_WIDTH, &v);
    shape.height = v;
  }

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_HEIGHT, &v);
    shape.width = v;
  }

  {
    GLint v;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, MIP_LEVEL, GL_TEXTURE_INTERNAL_FORMAT, &v);
    channels = channels_from_gl(v);
  }

  const std::size_t bytes = (shape.height * shape.width) * byte_count(typecode) * channels_to_count(channels);

  void* const data = std::malloc(bytes);
  glGetTexImage(GL_TEXTURE_2D, MIP_LEVEL, channels_to_gl(channels), to_gl_typecode(typecode), data);

  return std::make_tuple(data, bytes);
}

}  // namespace anonymous

TextureView::TextureView(
  void* const data,
  const Shape2D& shape,
  const TypeCode typecode,
  const TextureChannels channels) :
    data_{data},
    size_{(shape.height * shape.width) * byte_count(typecode) * channels_to_count(channels)},
    shape_{shape},
    typecode_{typecode},
    channels_{channels}
{}

TextureView::TextureView(std::uint8_t* const data, const Shape2D& shape, const TextureChannels channels) :
    TextureView{reinterpret_cast<void*>(data), shape, typecode<std::uint8_t>(), channels}
{}

TextureView::TextureView(std::uint16_t* const data, const Shape2D& shape, const TextureChannels channels) :
    TextureView{reinterpret_cast<void*>(data), shape, typecode<std::uint16_t>(), channels}
{}

TextureView::TextureView(std::uint32_t* const data, const Shape2D& shape, const TextureChannels channels) :
    TextureView{reinterpret_cast<void*>(data), shape, typecode<std::uint32_t>(), channels}
{}

TextureView::TextureView(float* const data, const Shape2D& shape, const TextureChannels channels) :
    TextureView{reinterpret_cast<void*>(data), shape, typecode<float>(), channels}
{}

TextureHost::TextureHost(const TextureHandle& texture) : TextureHost{texture.download()} {}

TextureHost::TextureHost(
  void* const data,
  const Shape2D& shape,
  const TypeCode typecode,
  const TextureChannels channels) :
    TextureView{data, shape, typecode, channels}
{}

TextureHost::~TextureHost()
{
  if (data_ != nullptr)
  {
    std::free(data_);
  }
}

TextureHandle::TextureHandle(TextureHandle&& other) : TextureHandle{other.texture_id_, other.typecode_, other.shape_}
{
  other.typecode_ = TypeCode::Invalid;
  other.shape_ = {};
}

TextureHandle::TextureHandle(const texture_id_t id, const TypeCode typecode, const Shape2D& shape) :
    texture_id_{id}, typecode_{typecode}, shape_{shape}
{}

void TextureHandle::upload(const TextureView& texture_data, const TextureOptions& texture_options) const
{
  TYL_ASSERT_NE(this->get_id(), 0);

  glBindTexture(GL_TEXTURE_2D, this->get_id());
  upload_gl_texture_2d(
    texture_data.shape(), texture_data.data(), texture_data.channels(), texture_options, texture_data.type());
  glBindTexture(GL_TEXTURE_2D, 0);
}

TextureHost TextureHandle::download() const
{
  TYL_ASSERT_NE(typecode_, TypeCode::Invalid);

  TextureHost texture_host;

  glBindTexture(GL_TEXTURE_2D, texture_id_);

  std::tie(texture_host.data_, texture_host.size_) =
    download_gl_texture_image(texture_host.shape_, texture_host.channels_, typecode_);

  texture_host.typecode_ = typecode_;

  return texture_host;
}

TextureHost TextureHandle::download(TextureOptions& options) const
{
  TYL_ASSERT_NE(typecode_, TypeCode::Invalid);

  TextureHost texture_host;

  glBindTexture(GL_TEXTURE_2D, texture_id_);

  download_gl_texture_options(options);

  std::tie(texture_host.data_, texture_host.size_) =
    download_gl_texture_image(texture_host.shape_, texture_host.channels_, typecode_);

  texture_host.typecode_ = typecode_;

  return texture_host;
}

TextureHandle& TextureHandle::operator=(TextureHandle&& other)
{
  new (this) TextureHandle{std::move(other)};
  return *this;
}

void TextureHandle::bind() const
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

  glBindTexture(GL_TEXTURE_2D, texture_id_);
}


void TextureHandle::bind(const index_t texture_index) const
{
  TextureHandle::bind();

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
}

void TextureHandle::unbind() const { TYL_ASSERT_NE(typecode_, TypeCode::Invalid); }

Texture::Texture(Texture&& other) : TextureHandle{std::move(static_cast<TextureHandle&&>(other))} {}

Texture::Texture(
  const Shape2D& shape,
  const TypeCode type,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_empty_texture_2d(shape, channels, options, type), type, shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::int8_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::int8_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::uint8_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::uint8_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::int16_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::int16_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::uint16_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::uint16_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::int32_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::int32_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const std::uint32_t* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<std::uint32_t>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const float* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<float>(), shape}
{}

Texture::Texture(
  const Shape2D& shape,
  const double* const data,
  const TextureChannels channels,
  const TextureOptions& options) :
    TextureHandle{create_gl_texture_2d(shape, data, channels, options), typecode<double>(), shape}
{}

Texture::Texture(const TextureView& texture_data, const TextureOptions& texture_options) :
    TextureHandle{
      create_gl_texture_2d<std::uint8_t>(
        texture_data.shape_,
        reinterpret_cast<const std::uint8_t*>(texture_data.data_),
        texture_data.channels_,
        texture_options,
        texture_data.typecode_),
      texture_data.typecode_,
      texture_data.shape_}
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
