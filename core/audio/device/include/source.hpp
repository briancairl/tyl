/**
 * @copyright 2022-present Brian Cairl
 *
 * @file source.hpp
 */
#pragma once

// Tyl
#include <tyl/audio/device/fwd.hpp>
#include <tyl/audio/device/typedef.hpp>

namespace tyl::audio::device
{

/**
 * @brief Sound playback encapsulation
 */
class Playback
{
public:
  Playback(Playback&& other);

  Playback(const Playback&) = delete;

  Playback(const Source& source, const Sound& sound);

  /**
   * @brief Fully stops playback when out of scope
   */
  ~Playback();

  /**
   * @brief Returns \c true if sound is currently being played by a sound device
   */
  [[nodiscard]] bool is_playing() const;

  /**
   * @brief Returns \c true if sound is currently paused during playback
   */
  [[nodiscard]] bool is_paused() const;

  /**
   * @brief Returns \c true if sound is currently stopped
   */
  [[nodiscard]] bool is_stopped() const;

  /**
   * @brief Resets sound to start of playback
   */
  void restart() const;

  /**
   * @brief Pauses current playback
   */
  void pause() const;

  /**
   * @brief Resumes current playback
   */
  void resume() const;

  /**
   * @brief Fully stops playback
   */
  void stop() const;

  /**
   * @brief Returns playback progress as a value between 0 and 1
   */
  [[nodiscard]] float progress() const;

  /**
   * @brief Returns true if Playback is valid
   */
  [[nodiscard]] constexpr bool is_valid() const { return playback_source_ != kInvalidSourceHandle; }

private:
  source_handle_t playback_source_;
  buffer_handle_t playback_buffer_;
  std::size_t playback_buffer_length_;

  friend class Source;
};

/**
 * @brief Audio source wrapper
 */
class Source
{
public:
  Source(Source&& other);

  Source(const Source&) = delete;

  Source();

  ~Source();

  /**
   * @brief Sets the volume of the sound source, between [0, 1]
   *
   *        At a volume of 1, the source will match the host system volume
   */
  void set_volume(const float volume) const;

  /**
   * @brief Scales normal pitch of the sound
   *
   *        - A value of 0.5 will lower the pitch of the sound an octave and slow playback by half
   *        - A value of 2.0 will raise the pitch of the sound an octave and speed up playback by double
   */
  void set_pitch_scaling(const float pitch_scaling) const;

  /**
   * @brief Sets the position of the sound source in the current audio field context
   *
   * @warning Only applies for single-channel (mono) sounds
   */
  void set_position(const float px, const float py, const float pz) const;

  /**
   * @brief Sets the velocity of the sound source in the current audio field context
   *
   * @warning Only applies for single-channel (mono) sounds
   */
  void set_velocity(const float vx, const float vy, const float vz) const;

  /**
   * @brief Sets sound to looped mode (indefinite repeat)
   */
  void set_looped(const bool looped) const;

  /**
   * @brief Plays a sound
   *
   * @note input as lvalue reference prevents rvalue/binding temporaries
   */
  [[nodiscard]] Playback play(Sound& sound) const;

  /**
   * @brief Returns true if Playback is valid
   */
  [[nodiscard]] constexpr bool is_valid() const { return source_ != kInvalidSourceHandle; }

  /**
   * @brief Returns opaque native handle to source
   */
  [[nodiscard]] constexpr source_handle_t get_source_handle() const { return source_; }

private:
  source_handle_t source_;
};

}  // namespace tyl::audio::device
