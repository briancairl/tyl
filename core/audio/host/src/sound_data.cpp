/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sound.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>
#include <memory>

// LibAudio
#include <audio/wave.h>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/audio/host/sound_data.hpp>

namespace tyl::audio::host
{
namespace
{

template <typename T, typename D> std::unique_ptr<T, D> make_wave_uptr(T* p, D deleter)
{
  return {p, std::move(deleter)};
}

}  // namespace

expected<SoundData, SoundData::Error> SoundData::create(
  void* data,
  const std::size_t buffer_length,
  const std::size_t bits_per_second,
  const ChannelFormat& channel_format)
{
  TYL_ASSERT_NON_NULL(data);

  if ((channel_format.count != 1) and (channel_format.count != 2))
  {
    return make_unexpected(Error::kInvalidChannelCount);
  }

  if (channel_format.bit_depth == 0)
  {
    return make_unexpected(Error::kInvalidChannelBitDepth);
  }

  return SoundData{data, buffer_length, bits_per_second, channel_format};
}

SoundData::SoundData(SoundData&& other) :
    data_{other.data_},
    buffer_length_{other.buffer_length_},
    bits_per_second_{other.bits_per_second_},
    channel_format_{other.channel_format_}
{
  other.data_ = nullptr;
}

SoundData::SoundData(
  void* const data,
  const std::size_t buffer_length,
  const std::size_t bits_per_second,
  const ChannelFormat& channel_format) :
    data_{data}, buffer_length_{buffer_length}, bits_per_second_{bits_per_second}, channel_format_{channel_format}
{}

SoundData::~SoundData()
{
  if (data_ == nullptr)
  {
    return;
  }
  std::free(data_);
}

device::Sound SoundData::sound() const { return {data_, buffer_length_, bits_per_second_, channel_format_}; }

expected<SoundData, SoundData::Error> SoundData::load(const std::filesystem::path& path)
{
  // Read WAV meta information
  auto wave = make_wave_uptr(WaveOpenFileForReading(path.c_str()), [](WaveInfo* wave_ptr) { WaveCloseFile(wave_ptr); });
  if (wave == nullptr)
  {
    return make_unexpected(Error::kInvalidSoundFile);
  }

  // Seek WAV to start
  if (const auto retcode = WaveSeekFile(0, wave.get()); retcode != 0)
  {
    return make_unexpected(Error::kInvalidSeek);
  }

  // Read WAV data
  auto* wave_data = reinterpret_cast<char*>(std::malloc(wave->dataSize));
  if (const auto read_size = WaveReadFile(wave_data, wave->dataSize, wave.get()); read_size != wave->dataSize)
  {
    return make_unexpected(Error::kInvalidReadSize);
  }

  return SoundData::create(
    wave_data,
    wave->dataSize,
    wave->sampleRate,
    ChannelFormat{
      .count = static_cast<std::uint32_t>(wave->channels),
      .bit_depth = static_cast<std::uint32_t>(wave->bitsPerSample)});
}

}  // namespace tyl::audio::host
