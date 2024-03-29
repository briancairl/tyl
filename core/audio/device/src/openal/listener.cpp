/**
 * @copyright 2022-present Brian Cairl
 *
 * @file listener.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>

// Tyl
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/device.hpp>
#include <tyl/audio/device/listener.hpp>

namespace tyl::audio::device
{

Listener::Listener(const Device& device) :
    listener_{
      reinterpret_cast<ALCcontext*>(alcCreateContext(reinterpret_cast<ALCdevice*>(device.get_device_handle()), NULL))}
{
  TYL_ASSERT_NON_NULL(listener_);

  // Set current context
  [[maybe_unused]] const bool success = alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(listener_));
  TYL_ASSERT_TRUE(success);

  // Set some reasonable defaults
  TYL_AL_TEST_ERROR(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));  // centered
  TYL_AL_TEST_ERROR(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));  // stationary
  const ALfloat listener_orientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
  TYL_AL_TEST_ERROR(alListenerfv(AL_ORIENTATION, listener_orientation));
}

Listener::Listener(Listener&& other) : listener_{other.listener_} { other.listener_ = kInvalidListenerHandle; }

Listener::~Listener()
{
  if (Listener::is_valid())
  {
    alcDestroyContext(reinterpret_cast<ALCcontext*>(listener_));
  }
}

void Listener::set_position(const float x, const float y, const float z) const
{
  TYL_ASSERT_TRUE(Listener::is_valid());
  [[maybe_unused]] const bool success = alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(listener_));
  TYL_ASSERT_TRUE(success);
  TYL_AL_TEST_ERROR(alListener3f(AL_POSITION, x, y, z));
}

void Listener::set_velocity(const float x, const float y, const float z) const
{
  TYL_ASSERT_TRUE(Listener::is_valid());
  [[maybe_unused]] const bool success = alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(listener_));
  TYL_ASSERT_TRUE(success);
  TYL_AL_TEST_ERROR(alListener3f(AL_VELOCITY, x, y, z));
}

void Listener::set_orientation(
  const float vx,
  const float vy,
  const float vz,
  const float ix,
  const float iy,
  const float iz) const
{
  TYL_ASSERT_TRUE(Listener::is_valid());
  [[maybe_unused]] const bool success = alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(listener_));
  TYL_ASSERT_TRUE(success);
  const ALfloat listener_orientation[] = {vx, vy, vz, ix, iy, iz};
  TYL_AL_TEST_ERROR(alListenerfv(AL_ORIENTATION, listener_orientation));
}

}  // namespace tyl::audio::device
