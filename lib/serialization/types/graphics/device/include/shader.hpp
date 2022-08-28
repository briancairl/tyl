/**
 * @copyright 2022-present Brian Cairl
 *
 * @file shader.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/graphics/device/shader.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>graphics::device::ShaderProgramHost</code> save implementation
 */
template <typename ArchiveT> struct save<ArchiveT, graphics::device::ShaderProgramHost>
{
  void operator()(ArchiveT& ar, const graphics::device::ShaderProgramHost& shader)
  {
    ar << named{"format", shader.format()};
    ar << named{"size", shader.size()};
    ar << named{"data", make_packet(shader.data(), shader.size())};
  }
};

/**
 * @brief Archive-generic <code>graphics::device::ShaderProgramHost</code> load implementation
 */
template <typename ArchiveT> struct load<ArchiveT, bypass_default_constructor<graphics::device::ShaderProgramHost>>
{
  void operator()(ArchiveT& ar, bypass_default_constructor<graphics::device::ShaderProgramHost>& shader)
  {
    graphics::device::enum_t format;
    ar >> named{"format", format};

    std::size_t size;
    ar >> named{"size", size};

    auto buffer = std::make_unique<std::uint8_t[]>(size);
    ar >> named{"data", make_packet(buffer.get(), size)};

    shader.construct(std::move(buffer), size, format);
  }
};

/**
 * @brief Archive-generic <code>graphics::device::Shader</code> save implementation
 */
template <typename ArchiveT> struct save<ArchiveT, graphics::device::Shader>
{
  void operator()(ArchiveT& ar, const graphics::device::Shader& shader)
  {
    const auto host_shader = shader.download();
    ar << named{"host", host_shader};
  }
};

/**
 * @brief Archive-generic <code>graphics::device::Shader</code> load implementation
 */
template <typename ArchiveT> struct load<ArchiveT, bypass_default_constructor<graphics::device::Shader>>
{
  void operator()(ArchiveT& ar, bypass_default_constructor<graphics::device::Shader>& shader)
  {
    bypass_default_constructor<graphics::device::ShaderProgramHost> host_shader;
    ar >> named{"host", host_shader};
    texture.construct(host_shader);
  }
};

}  // namespace tyl::serialization
