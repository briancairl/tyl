/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sound_data.hpp
 */
#pragma once

// C++ Standard Library
#include <cstddef>
#include <filesystem>
#include <memory>

// Tyl
#include <tyl/audio/device/sound.hpp>
#include <tyl/expected.hpp>

namespace tyl::audio::host
{

/**
 * @brief Sound data stored on host system
 */
class SoundData
{
public:
  using ChannelFormat = device::ChannelFormat;

  enum class Error
  {
    kInvalidSoundFile,
    kInvalidSeek,
    kInvalidReadSize,
    kInvalidChannelCount,
    kInvalidChannelBitDepth
  };

  SoundData(const SoundData&) = delete;

  SoundData(SoundData&& other);

  ~SoundData();

  /**
   * @brief Transfers sound to device
   */
  device::Sound sound() const;

  /**
   * @brief Instances sound from sound data
   */
  static expected<SoundData, Error> create(
    void* data,
    const std::size_t data_length,
    const std::size_t bits_per_second,
    const ChannelFormat& channel_format);

  /**
   * @brief Loads a sound from a file
   */
  static expected<SoundData, Error> load(const std::filesystem::path& path);

  /**
   * @brief Number of bytes used to store sound data
   */
  constexpr std::size_t size_in_bytes() const { return data_length_; };

  /**
   * @brief Intended bit-rate of sound data
   */
  constexpr std::size_t bit_rate() const { return bits_per_second_; };

  /**
   * @brief Intended sound channel format
   */
  constexpr const ChannelFormat& channel_format() { return channel_format_; };

private:
  /**
   * @brief Instances sound from sound data
   */
  SoundData(
    void* data,
    const std::size_t data_length,
    const std::size_t bits_per_second,
    const ChannelFormat& channel_format);

  /// Sound data buffer
  void* data_ = nullptr;
  /// Length of buffer, in bytes
  std::size_t data_length_ = 0;
  /// Bit rate
  std::size_t bits_per_second_ = 0;
  /// Sound channel formatting
  ChannelFormat channel_format_;
};

}  // namespace tyl::audio::host
