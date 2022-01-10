/**
 * @copyright 2022-present Brian Cairl
 *
 * @file source.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/device/source.hpp>

namespace tyl::audio::device
{

Playback::Playback(const source_handle_t source, const source_handle_t buffer) :
    playback_source_{source},
    playback_buffer_{buffer}
{
  alSourcei(playback_source_, AL_BUFFER, playback_buffer_);
  alSourcePlay(playback_source_);
}

Playback::~Playback() { Playback::stop(); }

bool Playback::is_playing() const
{
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_PLAYING;
}

bool Playback::is_stopped() const
{
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_STOPPED;
}

bool Playback::is_paused() const
{
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_PAUSED;
}

void Playback::restart() const
{
  TYL_AL_TEST_ERROR(alSourceRewind(playback_source_));
  TYL_AL_TEST_ERROR(alSourcePlay(playback_source_));
}

void Playback::pause() const { TYL_AL_TEST_ERROR(alSourcePause(playback_source_)); }

void Playback::stop() const { TYL_AL_TEST_ERROR(alSourceStop(playback_source_)); }

void Playback::resume() const { TYL_AL_TEST_ERROR(alSourcePlay(playback_source_)); }

Source::Source()
{
  alGenSources(1, &source_);

  // Set some reasonable defaults
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_GAIN, 1.f));  // device volume
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_PITCH, 1.f));  // default pitch
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_POSITION, 0.f, 0.f, 0.f));  // centered
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_VELOCITY, 0.f, 0.f, 0.f));  // stationary
  TYL_AL_TEST_ERROR(alSourcei(source_, AL_LOOPING, false));  // one-shot
}

Source::~Source() { alDeleteSources(1, &source_); }

void Source::set_volume(const float volume) const { TYL_AL_TEST_ERROR(alSourcef(source_, AL_GAIN, volume)); }

void Source::set_pitch_scaling(const float pitch_scaling) const
{
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_PITCH, pitch_scaling));
}

void Source::set_position(const float px, const float py, const float pz) const
{
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_POSITION, px, py, pz));
}

void Source::set_velocity(const float vx, const float vy, const float vz) const
{
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_VELOCITY, vx, vy, vz));
}

void Source::set_looped(const bool looped) const { TYL_AL_TEST_ERROR(alSourcei(source_, AL_LOOPING, looped)); }

Playback Source::play(const Sound& sound) { return Playback{source_, sound.get_buffer_handle()}; }

}  // namespace tyl::audio::device
