/**
 * @copyright 2022-present Brian Cairl
 *
 * @file source.hpp
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
  std::uint32_t count;

  /// Number of bits per channel data element
  std::uint32_t bit_depth;
};

/**
 * @brief A sound buffer
 */
class Sound
{
public:
  Sound();
  ~Sound();

  /**
   * @brief Instances sound from sound data
   */
  Sound(
    void* const data,
    const std::size_t data_length,
    const std::int32_t bits_per_second,
    const ChannelFormat& format);

  /**
   * @brief Transfers sound data to active buffer
   */
  void set_data(
    void* const data,
    const std::size_t data_length,
    const std::int32_t bits_per_second,
    const ChannelFormat& format);

  /**
   * @brief Returns opaque native handle to sound buffer
   */
  constexpr buffer_handle_t get_buffer_handle() const { return buffer_; }

private:
  buffer_handle_t buffer_;
};

/**
 * @brief Loads a sound from a file
 */
Sound load_sound_from_file(const char* filename);

}  // namespace tyl::audio::device
