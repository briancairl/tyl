/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/graphics/device/texture.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

/**
 * @brief Archive-generic <code>graphics::device::TextureHost</code> save implementation
 */
template <typename ArchiveT> struct save<ArchiveT, graphics::device::TextureHost>
{
  void operator()(ArchiveT& ar, const graphics::device::TextureHost& host_texture)
  {
    ar << named{"height", host_texture.height()};
    ar << named{"width", host_texture.width()};
    ar << named{"type", host_texture.type()};
    ar << named{"channels", host_texture.channels()};

    ar << named{"size", host_texture.size()};
    ar << named{"data", make_packet(host_texture.data(), host_texture.size())};
  }
};

/**
 * @brief Archive-generic <code>graphics::device::TextureHost</code> load implementation
 */
template <typename ArchiveT> struct load<ArchiveT, bypass_default_constructor<graphics::device::TextureHost>>
{
  void operator()(ArchiveT& ar, bypass_default_constructor<graphics::device::TextureHost>& host_texture)
  {
    int height, width;
    ar >> named{"height", height};
    ar >> named{"width", width};

    graphics::device::TypeCode type;
    ar >> named{"type", type};
    graphics::device::TextureChannels channels;
    ar >> named{"channels", channels};

    std::size_t size;
    ar >> named{"size", size};

    auto buffer = std::make_unique<std::uint8_t[]>(size);
    ar >> named{"data", make_packet(buffer.get(), size)};

    host_texture.construct(std::move(buffer), height, width, type, channels);
  }
};

/**
 * @brief Archive-generic <code>graphics::device::Texture</code> save implementation
 */
template <typename ArchiveT> struct save<ArchiveT, graphics::device::Texture>
{
  void operator()(ArchiveT& ar, const graphics::device::Texture& texture)
  {
    const auto host_texture = texture.download();
    ar << named{"host", host_texture};
  }
};

/**
 * @brief Archive-generic <code>graphics::device::Texture</code> load implementation
 */
template <typename ArchiveT> struct load<ArchiveT, bypass_default_constructor<graphics::device::Texture>>
{
  void operator()(ArchiveT& ar, bypass_default_constructor<graphics::device::Texture>& texture)
  {
    bypass_default_constructor<graphics::device::TextureHost> host_texture;
    ar >> named{"host", host_texture};
    texture.construct(host_texture);
  }
};

}  // namespace tyl::serialization
