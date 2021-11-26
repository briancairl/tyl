/**
 * @copyright 2020-present Brian Cairl
 *
 * @file vertex_buffer.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <vector>

// Art
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typecode.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

/**
 * @brief Specifies attributes about a vertex, used for creating VertexBuffer objects
 */
struct VertexAttributeDescriptor
{
  VertexAttributeDescriptor() = default;

  VertexAttributeDescriptor(const VertexAttributeDescriptor&) = default;

  VertexAttributeDescriptor(
    const TypeCode _type,
    const std::size_t _elements,
    const std::size_t _count = 0,
    const std::size_t _instance_divisor = 0,
    const bool _normalized = false) :
      type{_type},
      elements{_elements},
      count{_count},
      normalized{_normalized},
      instance_divisor{_instance_divisor}
  {}

  /**
   * @brief Returns number of bytes occupied by a single element of this attribute in a vertex buffer
   */
  inline std::size_t stride_bytes() const { return elements * byte_count(type); }

  /**
   * @brief Returns total number of bytes occupied by this this attribute in a vertex buffer
   */
  inline std::size_t total_bytes() const { return stride_bytes() * count; }

  /// Type code to specify the type of each element
  TypeCode type;

  /// Number of elements for attribute, e.g. "Vec2" := "2 elements"
  std::size_t elements;

  /// Total number of attributes to be stored
  std::size_t count;

  /// Specifies whether or not full attribute is normalized over all elements
  bool normalized;

  /// How many times this attribute is used per instance
  std::size_t instance_divisor;
};

/**
 * @brief RAII wrapper around a vertex buffer pointer
 */
class MappedBufferPtr
{
public:
  ~MappedBufferPtr();

  template <typename T> constexpr T* const as() { return reinterpret_cast<T* const>(ptr_); }

private:
  explicit MappedBufferPtr(const unsigned buffer_type, const unsigned buffer_mode, const std::size_t byte_offset);

  /// Type of buffer being mapped
  unsigned buffer_type_;

  /// Pointer to buffer (offset)
  void* const ptr_;

  friend class VertexBuffer;
};

/**
 * @brief RAII wrapper around a vertex resource
 *
 *        Creates and destroys vertex vertex/index buffers through graphics API
 */
class VertexBuffer
{
public:
  enum class BufferMode
  {
    STATIC,
    DYNAMIC,
  };

  enum class DrawMode
  {
    POINTS,
    LINES,
    LINE_STRIP,
    TRIANGLES,
  };

  VertexBuffer(
    const std::size_t index_count,
    const std::initializer_list<VertexAttributeDescriptor> vertex_attributes,
    const BufferMode draw_mode = BufferMode::STATIC);

  VertexBuffer(
    const std::initializer_list<VertexAttributeDescriptor> vertex_attributes,
    const BufferMode draw_mode = BufferMode::STATIC);

  VertexBuffer(VertexBuffer&& other);

  VertexBuffer& operator=(VertexBuffer&& other);

  void draw(const DrawMode mode = DrawMode::TRIANGLES) const;

  void draw_instanced(const std::size_t instance_count, const DrawMode mode = DrawMode::TRIANGLES) const;

  void set_vertex_data(const std::size_t attr_index, const float* const data) const;

  void set_vertex_data(const std::size_t attr_index, const std::int32_t* const data) const;

  void set_vertex_data(const std::size_t attr_index, const std::uint32_t* const data) const;

  void set_index_data(const std::uint32_t* const data) const;

  MappedBufferPtr get_vertex_ptr(const std::size_t attr_index) const;

  MappedBufferPtr get_index_ptr() const;

  ~VertexBuffer();

private:
  void set_vertex_data(const std::size_t attr_index, const void* const data) const;

  VertexBuffer(const VertexBuffer&) = default;

  vertex_buffer_id_t vao_;
  vertex_buffer_id_t vbo_;
  std::optional<vertex_buffer_id_t> ebo_;
  std::size_t index_count_;
  std::vector<VertexAttributeDescriptor> vertex_attributes_;

  friend class VertexBufferHandle;
};

}  // namespace tyl::graphics::device
