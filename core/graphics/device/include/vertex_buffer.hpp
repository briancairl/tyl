/**
 * @copyright 2020-present Brian Cairl
 *
 * @file vertex_buffer.hpp
 */
#pragma once

// C++ Standard Library
#include <array>
#include <cstdint>
#include <tuple>

// Tyl
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/fwd.hpp>
#include <tyl/graphics/device/typecode.hpp>
#include <tyl/graphics/device/typedef.hpp>

namespace tyl::graphics::device
{

enum class VertexAccessMode
{
  Direct,  ///< Specifies that fixed-point data values converted directly as fixed-point values when they are accessed
  Normalized  ///< Specifies that fixed-point data values should be normalized when they are accessed
};

template <
  typename ElementT,
  std::size_t ElementCount,
  std::size_t InstanceDivisor = 0,
  VertexAccessMode AccessMode = VertexAccessMode::Direct>
struct VertexAttribute
{
  /// Vertex count
  std::size_t length;

  explicit VertexAttribute(const std::size_t _length) : length{_length} {}
};

template <typename VertexAttributeT> struct VertexAttributeTraits;

template <typename ElementT, std::size_t ElementCount, std::size_t InstanceDivisor, VertexAccessMode AccessMode>
struct VertexAttributeTraits<VertexAttribute<ElementT, ElementCount, InstanceDivisor, AccessMode>>
{
  /// Element type
  using element_type = ElementT;

  /// Number of elements per vertex of @c ElementT constituting this attribute
  static constexpr std::size_t elements = ElementCount;

  /// Instanced rendering divisor
  static constexpr std::size_t instance_divisor = InstanceDivisor;

  /// Value access mode. @see VertexAccessMode
  static constexpr VertexAccessMode access_mode = AccessMode;

  /**
   * @brief Describes the memory footprint of an attribute for a single vertex
   */
  static constexpr std::size_t total_bytes() { return sizeof(element_type) * elements; }
};

template <typename... Attributes> constexpr std::size_t total_bytes_per_vertex()
{
  return (VertexAttributeTraits<Attributes>::total_bytes() + ...);
}

struct VertexAttributeBufferLayout
{
  std::size_t index;
  std::size_t length;
  std::size_t byte_offset;
  std::size_t byte_length;
};

template <typename ElementT> struct VertexAttributeBuffer : VertexAttributeBufferLayout
{
  using element_type = ElementT;

  constexpr explicit VertexAttributeBuffer(const VertexAttributeBufferLayout& layout) :
      VertexAttributeBufferLayout{layout}
  {}
};

struct VertexElementBufferLayout
{
  std::size_t length;
};

/// Element indexing type, typically used for indicating vertices constituting triangles, lines, etc. in a mesh
using element_t = std::uint32_t;

/**
 * @brief RAII wrapper around a mapped device vertex buffer pointer
 */
class MappedBuffer
{
public:
  ~MappedBuffer();

  template <typename ElementT> ElementT* operator()(const VertexAttributeBuffer<ElementT>& attr_buffer) const
  {
    auto* const byte_offset_ptr = reinterpret_cast<std::uint8_t* const>(data_) + attr_buffer.byte_offset;
    return reinterpret_cast<ElementT* const>(byte_offset_ptr);
  }

  element_t* operator()(const VertexElementBufferLayout& attr_buffer) const
  {
    auto* const byte_offset_ptr = reinterpret_cast<std::uint8_t* const>(data_);
    return reinterpret_cast<element_t* const>(byte_offset_ptr);
  }

  template <typename ElementT, typename AccessFn>
  void access(const VertexAttributeBuffer<ElementT>& attr_buffer, AccessFn&& access_fn)
  {
    access_fn(MappedBuffer::operator()(attr_buffer));
  }

  template <typename AccessFn> void access(const VertexElementBufferLayout& element_buffer, AccessFn access_fn)
  {
    access_fn(MappedBuffer::operator()(element_buffer));
  }

  MappedBuffer(MappedBuffer&& other);

  MappedBuffer(const enum_t target, void* const data);

private:
  MappedBuffer(const MappedBuffer&) = delete;

  /// Type of buffer being mapped
  enum_t target_;

  /// Pointer to buffer (offset)
  void* data_;
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
    kStream,
    kStatic,
    kDynamic,
  };

  enum class DrawMode
  {
    kPoints,
    kLines,
    kLineStrip,
    kTriangles,
  };

  template <typename... VertexAttributes> static auto create(const BufferMode buffer_mode, VertexAttributes&&... attrs)
  {
    static constexpr std::size_t N = sizeof...(VertexAttributes);

    static_assert(N > 0, "Must specify at least one VertexAttribute");

    const Attributes props[N] = {Attributes{
      .typecode = typecode<typename VertexAttributeTraits<VertexAttributes>::element_type>(),
      .access = VertexAttributeTraits<VertexAttributes>::access_mode,
      .elements = VertexAttributeTraits<VertexAttributes>::elements,
      .instance_divisor = VertexAttributeTraits<VertexAttributes>::instance_divisor,
      .length = attrs.length,
    }...};

    VertexBuffer vertex_buffer{
      ((sizeof(typename VertexAttributeTraits<VertexAttributes>::element_type) *
        VertexAttributeTraits<VertexAttributes>::elements * attrs.length) +
       ...),
      buffer_mode};

    using ElementTypePack = std::tuple<typename VertexAttributeTraits<VertexAttributes>::element_type...>;

    std::array<VertexAttributeBufferLayout, N> attribute_buffers;
    vertex_buffer.setup_attributes(attribute_buffers.data(), props, N);

    return std::tuple_cat(
      std::make_tuple(std::move(vertex_buffer)),
      adapt_to_vab<ElementTypePack>(attribute_buffers, std::make_integer_sequence<std::size_t, N>{}));
  }

  VertexBuffer(VertexBuffer&& other);

  ~VertexBuffer();

  VertexBuffer& operator=(VertexBuffer&& other);

  MappedBuffer get_mapped_vertex_buffer_write() const;

  MappedBuffer get_mapped_vertex_buffer_read() const;

  MappedBuffer get_mapped_vertex_buffer() const;

  void set(const VertexAttributeBuffer<float>& layout, const float* const data) const
  {
    set(layout, static_cast<const void* const>(data));
  }

  void set(const VertexAttributeBuffer<std::int32_t>& layout, const std::int32_t* const data) const
  {
    set(layout, static_cast<const void* const>(data));
  }

  void set(const VertexAttributeBuffer<std::uint32_t>& layout, const std::uint32_t* const data) const
  {
    set(layout, static_cast<const void* const>(data));
  }

  void draw(const std::size_t count, const DrawMode mode = DrawMode::kTriangles, const float size = 1.f) const;

  void draw(
    const VertexAttributeBufferLayout& layout,
    const DrawMode mode = DrawMode::kTriangles,
    const float size = 1.f) const
  {
    VertexBuffer::draw(layout.length, mode, size);
  }

protected:
  struct Attributes
  {
    TypeCode typecode;
    VertexAccessMode access;
    std::size_t elements;
    std::size_t instance_divisor;
    std::size_t length;
  };

  VertexBuffer(const std::size_t buffer_total_bytes, const BufferMode buffer_mode);

  void setup_attributes(
    VertexAttributeBufferLayout* const vertex_attribute_buffers,
    const Attributes* const vertex_attributes,
    const std::size_t vertex_attribute_count);

  template <typename AttributeElementTypes, typename LayoutArrayT, std::size_t... Indices>
  static constexpr auto adapt_to_vab(const LayoutArrayT& arr, std::integer_sequence<std::size_t, Indices...> _)
  {
    return std::make_tuple(
      VertexAttributeBuffer<std::tuple_element_t<Indices, AttributeElementTypes>>(arr[Indices])...);
  }

  vertex_buffer_id_t vao_;
  vertex_buffer_id_t vbo_;

private:
  void set(const VertexAttributeBufferLayout& layout, const void* const data) const;
  VertexBuffer(const VertexBuffer&) = delete;
};

/**
 * @brief RAII wrapper around a vertex resource
 *
 *        Creates and destroys vertex vertex/index buffers through graphics API
 */
class VertexElementBuffer : public VertexBuffer
{
public:
  template <typename... VertexAttributes>
  static auto create(const BufferMode buffer_mode, const std::size_t element_count, VertexAttributes&&... attrs)
  {
    static constexpr std::size_t N = sizeof...(VertexAttributes);

    static_assert(N > 0, "Must specify at least one VertexAttribute");

    const Attributes props[N] = {Attributes{
      .typecode = typecode<typename VertexAttributeTraits<VertexAttributes>::element_type>(),
      .access = VertexAttributeTraits<VertexAttributes>::access_mode,
      .elements = VertexAttributeTraits<VertexAttributes>::elements,
      .instance_divisor = VertexAttributeTraits<VertexAttributes>::instance_divisor,
      .length = attrs.length,
    }...};

    VertexElementBuffer vertex_buffer{
      element_count,
      ((sizeof(typename VertexAttributeTraits<VertexAttributes>::element_type) *
        VertexAttributeTraits<VertexAttributes>::elements * attrs.length) +
       ...),
      buffer_mode};

    using ElementTypePack = std::tuple<typename VertexAttributeTraits<VertexAttributes>::element_type...>;

    std::array<VertexAttributeBufferLayout, N> attribute_buffers;
    vertex_buffer.setup_attributes(attribute_buffers.data(), props, N);

    return std::tuple_cat(
      std::make_tuple(std::move(vertex_buffer), VertexElementBufferLayout{element_count}),
      adapt_to_vab<ElementTypePack>(attribute_buffers, std::make_integer_sequence<std::size_t, N>{}));
  }

  VertexElementBuffer(VertexElementBuffer&& other);

  VertexElementBuffer& operator=(VertexElementBuffer&& other);

  MappedBuffer get_mapped_element_buffer_write() const;

  MappedBuffer get_mapped_element_buffer_read() const;

  MappedBuffer get_mapped_element_buffer() const;

  void set(const VertexElementBufferLayout& layout, const element_t* const data) const;

  void draw_instanced(
    const VertexElementBufferLayout& layout,
    const std::size_t instance_count,
    const DrawMode mode = DrawMode::kTriangles) const;

  void draw(const std::size_t count, const DrawMode mode = DrawMode::kTriangles) const;

  void draw(
    const VertexElementBufferLayout& layout,
    const DrawMode mode = DrawMode::kTriangles,
    const float size = 1.0f) const
  {
    VertexElementBuffer::draw(layout.length, mode, size);
  }

  ~VertexElementBuffer();

private:
  using VertexBuffer::create;
  using VertexBuffer::draw;

  VertexElementBuffer(
    const std::size_t element_count,
    const std::size_t buffer_total_bytes,
    const BufferMode buffer_mode);

  VertexElementBuffer(const VertexElementBuffer&) = delete;

  vertex_buffer_id_t ebo_;
};


}  // namespace tyl::graphics::device
