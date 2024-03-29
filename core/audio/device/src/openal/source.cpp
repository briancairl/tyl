/**
 * @copyright 2022-present Brian Cairl
 *
 * @file source.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>

// Tyl
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/device/source.hpp>

namespace tyl::audio::device
{

Playback::Playback(const Source& source, const Sound& sound) :
    playback_source_{source.get_source_handle()},
    playback_buffer_{sound.get_buffer_handle()},
    playback_buffer_length_{sound.get_buffer_length()}
{
  ALint previous_buffer;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_BUFFER, &previous_buffer));
  std::fprintf(stderr, "!!! %d : %d != %d\n", playback_source_, previous_buffer, playback_buffer_);
  if (previous_buffer != playback_buffer_)
  {
    TYL_AL_TEST_ERROR(alSourcei(playback_source_, AL_BUFFER, playback_buffer_));
  }
  Playback::restart();
}

Playback::Playback(Playback&& other) :
    playback_source_{other.playback_source_},
    playback_buffer_{other.playback_buffer_},
    playback_buffer_length_{other.playback_buffer_length_}
{
  other.playback_source_ = kInvalidSourceHandle;
  other.playback_buffer_ = kInvalidBufferHandle;
  other.playback_buffer_length_ = 0;
}

Playback::~Playback()
{
  if (Playback::is_valid())
  {
    Playback::stop();
    TYL_AL_TEST_ERROR(alSourcei(playback_source_, AL_BUFFER, kInvalidSourceHandle));

    ALint previous_buffer;
    TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_BUFFER, &previous_buffer));
    std::fprintf(stderr, "~~~ %d : %d != %d\n", playback_source_, previous_buffer, playback_buffer_);
    if (previous_buffer != playback_buffer_)
    {
      TYL_AL_TEST_ERROR(alSourcei(playback_source_, AL_BUFFER, playback_buffer_));
    }
  }
}

bool Playback::is_playing() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_PLAYING;
}

bool Playback::is_stopped() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_STOPPED;
}

bool Playback::is_paused() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  ALint source_state;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_SOURCE_STATE, &source_state));
  return source_state == AL_PAUSED;
}

void Playback::restart() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());

  ALint previous_buffer;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_BUFFER, &previous_buffer));

  // TYL_AL_TEST_ERROR(alSourceRewind(playback_source_));
  TYL_AL_TEST_ERROR(alSourcePlay(playback_source_));
}

void Playback::pause() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  TYL_AL_TEST_ERROR(alSourcePause(playback_source_));
}

void Playback::stop() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  TYL_AL_TEST_ERROR(alSourceStop(playback_source_));
}

void Playback::resume() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  TYL_AL_TEST_ERROR(alSourcePlay(playback_source_));
}

float Playback::progress() const
{
  TYL_ASSERT_TRUE(Playback::is_valid());
  ALint byte_offset;
  TYL_AL_TEST_ERROR(alGetSourcei(playback_source_, AL_BYTE_OFFSET, &byte_offset));
  return static_cast<float>(byte_offset) / static_cast<float>(playback_buffer_length_);
}

Source::Source()
{
  TYL_AL_TEST_ERROR(alGenSources(1, &source_));

  // Set some reasonable defaults
  TYL_AL_TEST_ERROR(alSourcei(source_, AL_LOOPING, false));  // one-shot
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_GAIN, 1.f));  // device volume
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_PITCH, 1.f));  // default pitch
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_POSITION, 0.f, 0.f, 0.f));  // centered
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_VELOCITY, 0.f, 0.f, 0.f));  // stationary
}

Source::Source(Source&& other) : source_{other.source_} { other.source_ = kInvalidSourceHandle; }

Source::~Source()
{
  if (Source::is_valid())
  {
    alDeleteSources(1, &source_);
  }
}

void Source::set_volume(const float volume) const
{
  TYL_ASSERT_TRUE(Source::is_valid());
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_GAIN, volume));
}

void Source::set_pitch_scaling(const float pitch_scaling) const
{
  TYL_ASSERT_TRUE(Source::is_valid());
  TYL_AL_TEST_ERROR(alSourcef(source_, AL_PITCH, pitch_scaling));
}

void Source::set_position(const float px, const float py, const float pz) const
{
  TYL_ASSERT_TRUE(Source::is_valid());
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_POSITION, px, py, pz));
}

void Source::set_velocity(const float vx, const float vy, const float vz) const
{
  TYL_ASSERT_TRUE(Source::is_valid());
  TYL_AL_TEST_ERROR(alSource3f(source_, AL_VELOCITY, vx, vy, vz));
}

void Source::set_looped(const bool looped) const
{
  TYL_ASSERT_TRUE(Source::is_valid());
  TYL_AL_TEST_ERROR(alSourcei(source_, AL_LOOPING, looped));
}

Playback Source::play(Sound& sound) const
{
  TYL_ASSERT_TRUE(sound.is_valid());
  return Playback{*this, sound};
}

}  // namespace tyl::audio::device
