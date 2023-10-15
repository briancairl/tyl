/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sound.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <memory>

// LibAudio
#include <audio/wave.h>

// Tyl
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/sound.hpp>

namespace tyl::audio::device
{
namespace  // anonymous
{

struct UnsupportedChannelFormat final : std::exception
{
  const char* what() const noexcept override
  {
    return "[OpenAL::UnsupportedChannelFormat] channel format be {MONO8, STEREO8, MONO16, STEREO16}";
  }
};

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
  throw UnsupportedChannelFormat{};
}

}  // namespace anonymous

Sound::Sound() { alGenBuffers(1, &buffer_); }

Sound::Sound(
  void* const data,
  const std::size_t data_length,
  const std::int32_t bits_per_second,
  const ChannelFormat& format) :
    Sound{}
{
  Sound::set_data(data, data_length, bits_per_second, format);
}

void Sound::set_data(
  void* const data,
  const std::size_t data_length,
  const std::int32_t bits_per_second,
  const ChannelFormat& format)
{
  TYL_AL_TEST_ERROR(alBufferData(buffer_, to_al_channel_format(format), data, data_length, bits_per_second));
}

Sound::~Sound() { alDeleteBuffers(1, &buffer_); }

Sound load_sound_from_file(const char* filename)
{
  // Read WAV meta information
  WaveInfo* const wave = WaveOpenFileForReading(filename);
  TYL_ASSERT(wave != nullptr, filename);

  // Seek WAV to start
  {
    const int retcode = WaveSeekFile(0, wave);
    TYL_ASSERT_EQ(retcode, 0);
  }

  // Read WAV data
  auto buffer_data = std::make_unique<char[]>(wave->dataSize);
  {
    const unsigned read_size = WaveReadFile(buffer_data.get(), wave->dataSize, wave);
    TYL_ASSERT_EQ(read_size, wave->dataSize);
  }

  Sound sound{
    reinterpret_cast<void*>(buffer_data.get()),
    wave->dataSize,
    wave->sampleRate,
    ChannelFormat{
      .count = static_cast<std::uint32_t>(wave->channels),
      .bit_depth = static_cast<std::uint32_t>(wave->bitsPerSample)}};

  // Close the file
  WaveCloseFile(wave);

  return sound;
}

}  // namespace tyl::audio::device
