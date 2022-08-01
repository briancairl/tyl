/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture_options.hpp
 */
#pragma once

namespace tyl::device::graphics
{

enum class TextureChannels
{
  R,  //< Red (1-channel)
  RG,  //< Red-green (2-channel)
  RGB,  //< Red-green-blue (3-channel)
  RGBA,  //< Red-green-blue-alpha (4-channel)
};

struct TextureOptions
{
  enum class Wrapping
  {
    CLAMP_TO_BORDER,
    REPEAT
  };

  enum class Sampling
  {
    LINEAR,
    NEAREST
  };

  Wrapping u_wrapping = Wrapping::CLAMP_TO_BORDER;

  Wrapping v_wrapping = Wrapping::CLAMP_TO_BORDER;

  Sampling min_sampling = Sampling::NEAREST;

  Sampling mag_sampling = Sampling::NEAREST;

  TextureOptions() = default;
};

}  // namespace tyl::device::graphics
