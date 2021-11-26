/**
 * @copyright 2021-present Brian Cairl
 *
 * @file vertex_buffer.hpp
 */
#pragma once

// Tyl
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/ref.hpp>

namespace tyl::graphics
{

/**
 * @brief VertexBuffer resource
 */
struct VertexBuffer : RefCounted<VertexBuffer>, device::VertexBuffer
{
  using Mode = device::VertexBuffer::BufferMode;
  using Attribute = device::VertexAttributeDescriptor;

  VertexBuffer(VertexBuffer&& other) = default;

  VertexBuffer(
    const std::size_t index_count,
    std::initializer_list<Attribute> vertex_attributes,
    const Mode draw_mode = Mode::STATIC) :
      RefCounted<VertexBuffer>{},
      device::VertexBuffer{index_count, std::move(vertex_attributes), draw_mode}
  {}

  VertexBuffer(std::initializer_list<Attribute> vertex_attributes, const Mode draw_mode = Mode::STATIC) :
      RefCounted<VertexBuffer>{},
      device::VertexBuffer{std::move(vertex_attributes), draw_mode}
  {}

  ~VertexBuffer() = default;
};

}  // namespace tyl::graphics
