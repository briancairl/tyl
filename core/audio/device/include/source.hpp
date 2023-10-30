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

  /**
   * @brief Fully stops playback when out of scope
   */
  ~Playback();

  /**
   * @brief Returns \c true if sound is currently being played by a sound device
   */
  bool is_playing() const;

  /**
   * @brief Returns \c true if sound is currently paused during playback
   */
  bool is_paused() const;

  /**
   * @brief Returns \c true if sound is currently stopped
   */
  bool is_stopped() const;

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
   * @brief Returns true if Playback is valid
   */
  constexpr bool is_valid() const { return playback_source_ != kInvalidSourceHandle; }

private:
  Playback(const source_handle_t source, const source_handle_t buffer);

  source_handle_t playback_source_;
  buffer_handle_t playback_buffer_;

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
   * @brief Sets the position of the sound sourc in the current audio field context
   *
   * @warning Only applies for single-channel (mono) sounds
   */
  void set_position(const float px, const float py, const float pz) const;

  /**
   * @brief Sets the velocity of the sound sourc in the current audio field context
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
   */
  Playback play(const Sound& sound);

  /**
   * @brief Returns true if Playback is valid
   */
  constexpr bool is_valid() const { return source_ != kInvalidSourceHandle; }

  /**
   * @brief Returns opaque native handle to source
   */
  constexpr source_handle_t get_source_handle() const { return source_; }

private:
  source_handle_t source_;
};

}  // namespace tyl::audio::device
