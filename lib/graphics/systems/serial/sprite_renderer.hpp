/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_renderer.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/graphics/systems/sprite_renderer.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/packet.hpp>

namespace tyl::serialization
{

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
