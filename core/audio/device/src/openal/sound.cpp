/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sound.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>

// Tyl
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/sound.hpp>

namespace tyl::audio::device
{
namespace  // anonymous
{
inline ALenum to_al_channel_format(const ChannelFormat& format)
{
  const bool stereo = (format.count == 2);
  switch (format.bit_depth)
  {
  case 8:
    return stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
  case 16:
    return stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
  default:
    break;
  }
  return 0;
}

}  // namespace anonymous

Sound::Sound(Sound&& other) : buffer_{other.buffer_} { other.buffer_ = 0; }

Sound::Sound() { alGenBuffers(1, &buffer_); }

Sound::Sound(
  void* const data,
  const std::size_t data_length,
  const std::size_t bits_per_second,
  const ChannelFormat& format) :
    Sound{}
{
  Sound::set_data(data, data_length, bits_per_second, format);
}

void Sound::set_data(
  void* const data,
  const std::size_t data_length,
  const std::size_t bits_per_second,
  const ChannelFormat& format)
{
  TYL_ASSERT_NE(buffer_, kInvalidBufferHandle);
  TYL_ASSERT_NON_NULL(data);
  TYL_AL_TEST_ERROR(alBufferData(buffer_, to_al_channel_format(format), data, data_length, bits_per_second));
}

Sound::~Sound()
{
  if (Sound::is_valid())
  {
    alDeleteBuffers(1, &buffer_);
  }
}

}  // namespace tyl::audio::device
