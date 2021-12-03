/**
 * @copyright 2021-present Brian Cairl
 *
 * @file vertex_buffer.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>

namespace tyl::graphics
{

/**
 * @brief VertexBuffer resource
 */
struct VertexBuffer : ecs::make_ref_from_this<VertexBuffer>, device::VertexBuffer
{
  using Mode = device::VertexBuffer::BufferMode;

  using Attribute = device::VertexAttributeDescriptor;

  VertexBuffer(VertexBuffer&& other) = default;
  VertexBuffer& operator=(VertexBuffer&&) = default;

  VertexBuffer(
    const std::size_t index_count,
    std::initializer_list<Attribute> vertex_attributes,
    const Mode draw_mode = Mode::STATIC) :
      ecs::make_ref_from_this<VertexBuffer>{},
      device::VertexBuffer{index_count, std::move(vertex_attributes), draw_mode}
  {}

  VertexBuffer(std::initializer_list<Attribute> vertex_attributes, const Mode draw_mode = Mode::STATIC) :
      ecs::make_ref_from_this<VertexBuffer>{},
      device::VertexBuffer{std::move(vertex_attributes), draw_mode}
  {}

  ~VertexBuffer() = default;
};

}  // namespace tyl::graphics
