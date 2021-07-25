/**
 * @copyright 2020-present Brian Cairl
 *
 * @file vertex_buffer.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <assert.h>
#include <iterator>
#include <numeric>

// Art
#include <tyl/graphics/gl.hpp>
#include <tyl/graphics/vertex_buffer.hpp>

namespace tyl::graphics
{

inline static GLuint to_gl_buffer_mode(const VertexBuffer::BufferMode mode)
{
  switch (mode)
  {
  case VertexBuffer::BufferMode::STATIC:
    return GL_STATIC_DRAW;
  case VertexBuffer::BufferMode::DYNAMIC:
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
  case VertexBuffer::DrawMode::POINTS:
    return GL_POINTS;
  case VertexBuffer::DrawMode::TRIANGLES:
    return GL_TRIANGLES;
  case VertexBuffer::DrawMode::LINE_STRIP:
    return GL_LINE_STRIP;
  case VertexBuffer::DrawMode::LINES:
    return GL_LINES;
  default:
    break;
  }
  return -1;
}

VertexBuffer::VertexBuffer(
  const std::size_t index_count,
  const std::initializer_list<VertexAttributeDescriptor> vertex_attributes,
  const BufferMode buffer_mode) :
    // clang-format off
    vao_{[]() -> vertex_buffer_id_t { GLuint id; glGenVertexArrays(1, &id); return id;}()},
    vbo_{[]() -> vertex_buffer_id_t { GLuint id; glGenBuffers(1, &id); return id; }()},
    ebo_{[index_count]() -> std::optional<vertex_buffer_id_t>
      {
        if (index_count)
        {
          GLuint id;
          glGenBuffers(1, &id);
          return id;
        }
        else
        {
          return std::nullopt;
        }
      }()
    },
    index_count_{index_count},
    vertex_attributes_{vertex_attributes}
// clang-format on
{
  glBindVertexArray(vao_);

  // Reserve vertex buffer
  {
    // Compute total number of bytes per vertex
    const std::size_t total_bytes = std::accumulate(
      vertex_attributes.begin(),
      vertex_attributes.end(),
      0UL,
      [](const std::size_t prev, const VertexAttributeDescriptor& attr) { return prev + attr.total_bytes(); });

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, total_bytes, 0, to_gl_buffer_mode(buffer_mode));
  }

  // Reserve index buffer
  if (ebo_)
  {
    static constexpr std::size_t bytes_per_index = sizeof(GLuint);
    const std::size_t total_bytes = index_count * bytes_per_index;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_bytes, 0, to_gl_buffer_mode(buffer_mode));
  }

  // Setup vertex buffer layout
  {
    std::size_t offset = 0UL;
    std::size_t layout_index = 0UL;
    for (const VertexAttributeDescriptor& attr : vertex_attributes)
    {
      static constexpr std::uint8_t* OFFSET_START = nullptr;

      glEnableVertexAttribArray(layout_index);

      glVertexAttribPointer(
        layout_index,  // layout index
        attr.elements,  // elementcount
        to_gl_typecode(attr.type),  // typecode
        to_gl_bool(attr.normalized),  // normalized
        attr.stride_bytes(),  // stride
        static_cast<const GLvoid*>(OFFSET_START + offset)  // offset in buffer
      );

      glVertexAttribDivisor(layout_index, attr.instance_divisor);

      // Next offset is after the last batch of vertex attribute data
      offset += attr.total_bytes();
      ++layout_index;
    }
  }

  glBindVertexArray(0);
}

VertexBuffer::VertexBuffer(
  const std::initializer_list<VertexAttributeDescriptor> vertex_attributes,
  const BufferMode buffer_mode) :
    VertexBuffer{0UL, vertex_attributes, buffer_mode}
{}

VertexBuffer::VertexBuffer(VertexBuffer&& other) :
    vao_{other.vao_},
    vbo_{other.vbo_},
    ebo_{std::move(other.ebo_)},
    index_count_{other.index_count_},
    vertex_attributes_{std::move(other.vertex_attributes_)}
{
  other.vertex_attributes_.clear();
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
{
  new (this) VertexBuffer{std::move(other)};
  return *this;
}

void VertexBuffer::set_vertex_data(const std::size_t attr_index, const void* const data) const
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  // Get iterator to vertex attribute
  const auto attr_itr = std::next(vertex_attributes_.begin(), attr_index);

  // Compute data offset before data to set
  const std::size_t data_offset = std::accumulate(
    vertex_attributes_.begin(), attr_itr, 0UL, [](const std::size_t prev, const VertexAttributeDescriptor& attr) {
      return prev + attr.total_bytes();
    });

  // Upload new data
  glBufferSubData(GL_ARRAY_BUFFER, data_offset, attr_itr->total_bytes(), data);
}

void VertexBuffer::set_vertex_data(const std::size_t attr_index, const float* const data) const
{
  assert(attr_index < vertex_attributes_.size());
  set_vertex_data(attr_index, static_cast<const void* const>(data));
}

void VertexBuffer::set_vertex_data(const std::size_t attr_index, const int* const data) const
{
  assert(attr_index < vertex_attributes_.size());
  set_vertex_data(attr_index, static_cast<const void* const>(data));
}

void VertexBuffer::set_index_data(const unsigned* const data) const
{
  static_assert(std::is_same<unsigned, GLuint>(), "'GUint != unsigned integer type");
  assert(ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo_);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * index_count_, data);
}

void VertexBuffer::draw(const DrawMode mode) const
{
  glBindVertexArray(vao_);
  if (ebo_)
  {
    glDrawElements(to_gl_draw_mode(mode), index_count_, GL_UNSIGNED_INT, 0);
  }
  else
  {
    glDrawArrays(to_gl_draw_mode(mode), 0, vertex_attributes_[0].count);
  }
  glBindVertexArray(0);
}

void VertexBuffer::draw_instanced(const std::size_t instance_count, const DrawMode mode) const
{
  glBindVertexArray(vao_);
  glDrawElementsInstanced(to_gl_draw_mode(mode), index_count_, GL_UNSIGNED_INT, 0, instance_count);
  glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer()
{
  if (vertex_attributes_.empty())
  {
    return;
  }

  {
    GLuint delete_array = vao_;
    glDeleteVertexArrays(1, &delete_array);
  }

  {
    GLuint delete_buffer = vbo_;
    glDeleteBuffers(1, &delete_buffer);
  }

  if (ebo_)
  {
    GLuint delete_buffer = *ebo_;
    glDeleteBuffers(1, &delete_buffer);
  }
}

}  // namespace tyl::graphics
