/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sound.hpp
 */
#pragma once

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/audio/device/fwd.hpp>
#include <tyl/audio/device/typedef.hpp>

namespace tyl::audio::device
{

/**
 * @brief Audio channel description
 */
struct ChannelFormat
{
  /// Number of channels e.g. (1 := mono, 2 := stereo)
  std::uint32_t count = 0;

  /// Number of bits per channel data element
  std::uint32_t bit_depth = 0;
};

/**
 * @brief A sound buffer
 */
class Sound
{
public:
  Sound(const Sound&) = delete;

  Sound(Sound&& other);

  Sound();

  ~Sound();

  /**
   * @brief Instances sound from sound data
   */
  Sound(
    void* const data,
    const std::size_t buffer_length,
    const std::size_t bits_per_second,
    const ChannelFormat& format);

  /**
   * @brief Transfers sound data to active buffer
   */
  void set_data(
    void* const data,
    const std::size_t buffer_length,
    const std::size_t bits_per_second,
    const ChannelFormat& format);

  /**
   * @brief Returns true if sound is valid
   */
  [[nodiscard]] constexpr bool is_valid() const { return buffer_ != kInvalidBufferHandle; }

  /**
   * @brief Returns opaque native handle to sound buffer
   */
  constexpr buffer_handle_t get_buffer_handle() const { return buffer_; }

  /**
   * @brief Returns the size of sound buffer on device, in bytes
   */
  constexpr std::size_t get_buffer_length() const { return buffer_length_; }

private:
  buffer_handle_t buffer_;
  std::size_t buffer_length_;
};

}  // namespace tyl::audio::device
