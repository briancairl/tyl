/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.hpp
 */
#pragma once

// Tyl
#include <tyl/common/vec.hpp>
#include <tyl/device/graphics/fwd.hpp>
#include <tyl/device/graphics/texture_options.hpp>

namespace tyl::graphics
{

/// Texture handle type
using Texture = device::graphics::Texture;

/// Texture preperation options
using TextureOptions = device::graphics::TextureOptions;

/// Texture channel mode e.g. (R, RG, RGB, etc.)
using TextureChannels = device::graphics::TextureChannels;

/**
 * @brief Meta information about a loaded texture
 */
struct TextureInfo
{
  /// Height and width of the image plane
  Vec2i size;

  /// Number of texture channels
  int channels;
};

}  // namespace tyl::graphics
