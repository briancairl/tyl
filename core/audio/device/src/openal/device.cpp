/**
 * @copyright 2022-present Brian Cairl
 *
 * @file device.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <cstdlib>

// Tyl
#include <tyl/audio/device/al.inl>
#include <tyl/audio/device/device.hpp>

namespace tyl::audio::device
{

Device::Device(const char* device_name) :
    device_{reinterpret_cast<device_handle_t>(alcOpenDevice(device_name))},
    context_{reinterpret_cast<context_handle_t>(alcCreateContext(reinterpret_cast<ALCdevice*>(device_), NULL))}
{
  TYL_ASSERT_NON_NULL(device_);
  TYL_ASSERT_NON_NULL(context_);
}

Device::Device() : Device{alcGetString(NULL, ALC_DEVICE_SPECIFIER)} {}

Device::Device(Device&& other) : device_{other.device_}, context_{other.context_}
{
  other.device_ = kInvalidDeviceHandle;
  other.context_ = kInvalidContextHandle;
}

Device::~Device()
{
  if (Device::is_valid())
  {
    TYL_ASSERT_TRUE(Device::is_valid());
    alcCloseDevice(reinterpret_cast<ALCdevice*>(device_));
    alcDestroyContext(reinterpret_cast<ALCcontext*>(context_));
  }
}

bool Device::enable() const
{
  TYL_ASSERT_TRUE(Device::is_valid());
  return alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(context_)) == ALC_TRUE;
}

bool Device::disable() const
{
  TYL_ASSERT_TRUE(Device::is_valid());
  return alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(kInvalidContextHandle)) == ALC_TRUE;
}

}  // namespace tyl::audio::device
