/**
 * @copyright 2023-present Brian Cairl
 *
 * @file types_fwd.hpp
 */
#pragma once

namespace tyl::audio
{
namespace host
{
class SoundData;
}  // namespace host

namespace device
{
class Sound;
}  // namespace device
}  // namespace tyl::audio


namespace tyl::graphics
{
namespace host
{
class Image;
}  // namespace host

namespace device
{
class Texture;
}  // namespace device
}  // namespace tyl::graphics

namespace tyl::engine::asset
{

// Audio types collated under common 'engine::asset' namespace
using SoundData = ::tyl::audio::host::SoundData;
using Sound = ::tyl::audio::device::Sound;

// Graphics types collated under common 'engine::asset' namespace
using Image = ::tyl::graphics::host::Image;
using Texture = ::tyl::graphics::device::Texture;

struct Collection;

template <typename AssetT> struct Location;

struct Info;

}  // namespace tyl::engine::asset
