/**
 * @copyright 2020-present Brian Cairl
 *
 * @file vertex_buffer.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <iterator>
#include <numeric>

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/device/graphics/gl.inl>
#include <tyl/device/graphics/vertex_buffer.hpp>

namespace tyl::graphics::device
{
namespace  // anonymous
{

inline static GLuint to_gl_buffer_mode(const VertexBuffer::BufferMode mode)
{
  switch (mode)
  {
  case VertexBuffer::BufferMode::Stream:
    return GL_STREAM_DRAW;
  case VertexBuffer::BufferMode::Static:
    return GL_STATIC_DRAW;
  case VertexBuffer::BufferMode::Dynamic:
    return GL_DYNAMIC_DRAW;
  default:
    break;
  }
  return -1;
}

inline static GLuint to_gl_draw_mode(const VertexBuffer::DrawMode mode)
{
  switch (mode)
  {
  case VertexBuffer::DrawMode::Points:
    return GL_POINTS;
  case VertexBuffer::DrawMode::Triangles:
    return GL_TRIANGLES;
  case VertexBuffer::DrawMode::LineStrip:
    return GL_LINE_STRIP;
  case VertexBuffer::DrawMode::Lines:
    return GL_LINES;
  default:
    break;
  }
  return -1;
}

template <GLenum Target, GLenum Access> MappedBuffer get_mapped_buffer(GLuint id)
{
  TYL_ASSERT_NE(id, 0);
  glBindBuffer(Target, id);
  auto* const mapped_buffer_ptr = reinterpret_cast<std::uint8_t*>(glMapBuffer(Target, Access));
  TYL_ASSERT_NON_NULL(mapped_buffer_ptr);
  return MappedBuffer{Target, reinterpret_cast<void*>(mapped_buffer_ptr)};
}

}  // namespace anonymous

MappedBuffer::MappedBuffer(const unsigned target, void* const data) : target_{target}, data_{data} {}

MappedBuffer::MappedBuffer(MappedBuffer&& other) : target_{other.target_}, data_{other.data_} { other.data_ = nullptr; }

MappedBuffer::~MappedBuffer()
{
  if (this->data_ != nullptr)
  {
    glUnmapBuffer(target_);
  }
}

VertexBuffer::VertexBuffer(const std::size_t buffer_total_bytes, const BufferMode buffer_mode) :
    vao_{[]() -> vertex_buffer_id_t {
      GLuint id;
      glGenVertexArrays(1, &id);
      return id;
    }()},
    vbo_{[]() -> vertex_buffer_id_t {
      GLuint id;
      glGenBuffers(1, &id);
      return id;
    }()}
{
  glBindVertexArray(vao_);

  // Reserve vertex attribute buffer (VBO)
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, buffer_total_bytes, nullptr, to_gl_buffer_mode(buffer_mode));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) : vao_{other.vao_}, vbo_{other.vbo_}
{
  other.vao_ = 0;
  other.vbo_ = 0;
}

VertexBuffer::~VertexBuffer()
{
  if (vao_ != 0)
  {
    GLuint delete_array = vao_;
    glDeleteVertexArrays(1, &delete_array);
  }

  if (vbo_ != 0)
  {
    GLuint delete_buffer = vbo_;
    glDeleteBuffers(1, &delete_buffer);
  }
}

void VertexBuffer::setup_attributes(
  VertexAttributeBufferLayout* const vertex_attribute_buffers,
  const Attributes* const vertex_attributes,
  const std::size_t vertex_attribute_count)
{
  TYL_ASSERT_NE(vao_, 0);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  auto* attributes = vertex_attributes;
  auto* attribute_buffer = vertex_attribute_buffers;

  std::size_t byte_total_offset = 0UL;
  for (std::size_t attr_index = 0; attr_index < vertex_attribute_count; ++attr_index, ++attribute_buffer, ++attributes)
  {
    static constexpr std::uint8_t* OFFSET_START = nullptr;

    const std::size_t bytes_per_vertex = attributes->elements * byte_count(attributes->typecode);
    const std::size_t byte_total = bytes_per_vertex * attributes->length;

    attribute_buffer->index = attr_index;
    attribute_buffer->length = attributes->length;
    attribute_buffer->byte_offset = byte_total_offset;
    attribute_buffer->byte_length = byte_total;

    glEnableVertexAttribArray(attr_index);

    glVertexAttribPointer(
      attr_index,  // layout index
      attributes->elements,  // elementcount
      to_gl_typecode(attributes->typecode),  // typecode
      to_gl_bool(attributes->access == VertexAccessMode::Normalized),  // normalized
      bytes_per_vertex,  // stride
      static_cast<const GLvoid*>(OFFSET_START + byte_total_offset)  // offset in buffer
    );

    glVertexAttribDivisor(attr_index, attributes->instance_divisor);

    // Next offset is after the last batch of vertex attribute data
    byte_total_offset += byte_total;
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
{
  new (this) VertexBuffer{std::move(other)};
  return *this;
}

MappedBuffer VertexBuffer::get_mapped_vertex_buffer_write() const
{
  return get_mapped_buffer<GL_ARRAY_BUFFER, GL_WRITE_ONLY>(vbo_);
}

MappedBuffer VertexBuffer::get_mapped_vertex_buffer_read() const
{
  return get_mapped_buffer<GL_ARRAY_BUFFER, GL_READ_ONLY>(vbo_);
}

MappedBuffer VertexBuffer::get_mapped_vertex_buffer() const
{
  return get_mapped_buffer<GL_ARRAY_BUFFER, GL_READ_WRITE>(vbo_);
}

void VertexBuffer::set(const VertexAttributeBufferLayout& layout, const void* const data) const
{
  TYL_ASSERT_NON_NULL(data);
  TYL_ASSERT_NE(vbo_, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, layout.byte_offset, layout.byte_length, data);
}

void VertexBuffer::draw(const std::size_t count, const DrawMode mode) const
{
  glBindVertexArray(vao_);
  glDrawArrays(to_gl_draw_mode(mode), 0, count);
  glBindVertexArray(0);
}

VertexElementBuffer::VertexElementBuffer(
  const std::size_t element_count,
  const std::size_t buffer_total_bytes,
  const BufferMode buffer_mode) :
    VertexBuffer{buffer_total_bytes, buffer_mode}, ebo_{[element_count]() -> vertex_buffer_id_t {
      GLuint id;
      glGenBuffers(1, &id);
      return id;
    }()}
{
  static constexpr std::size_t bytes_per_index = sizeof(GLuint);
  const std::size_t total_bytes = element_count * bytes_per_index;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_bytes, 0, to_gl_buffer_mode(buffer_mode));
}

VertexElementBuffer::VertexElementBuffer(VertexElementBuffer&& other) :
    VertexBuffer{std::move(static_cast<VertexBuffer&>(other))}, ebo_{other.ebo_}
{
  other.ebo_ = 0;
}

VertexElementBuffer::~VertexElementBuffer()
{
  static_cast<VertexBuffer&>(*this).~VertexBuffer();

  if (ebo_ != 0)
  {
    GLuint delete_buffer = ebo_;
    glDeleteBuffers(1, &delete_buffer);
  }
}

MappedBuffer VertexElementBuffer::get_mapped_element_buffer_write() const
{
  return get_mapped_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY>(ebo_);
}

MappedBuffer VertexElementBuffer::get_mapped_element_buffer_read() const
{
  return get_mapped_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY>(ebo_);
}

MappedBuffer VertexElementBuffer::get_mapped_element_buffer() const
{
  return get_mapped_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE>(ebo_);
}

void VertexElementBuffer::set(const VertexElementBufferLayout& layout, const element_t* const data) const
{
  static_assert(std::is_same<element_t, GLuint>(), "'GUint != element_t integer type");
  TYL_ASSERT_NE(ebo_, invalid_vertex_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, layout.length * sizeof(element_t), data);
}

void VertexElementBuffer::draw(const VertexElementBufferLayout& layout, const DrawMode mode) const
{
  glBindVertexArray(vao_);
  TYL_ASSERT_TRUE(ebo_);
  glDrawElements(to_gl_draw_mode(mode), layout.length, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void VertexElementBuffer::draw_instanced(
  const VertexElementBufferLayout& layout,
  const std::size_t instance_count,
  const DrawMode mode) const
{
  glBindVertexArray(vao_);
  glDrawElementsInstanced(to_gl_draw_mode(mode), layout.length, GL_UNSIGNED_INT, 0, instance_count);
  glBindVertexArray(0);
}

}  // namespace tyl::graphics::device
