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

Device::Device(const char* device_name) : device_{reinterpret_cast<device_handle_t>(alcOpenDevice(device_name))}
{
  TYL_ASSERT_NON_NULL(device_);
}

Device::Device() : Device{alcGetString(NULL, ALC_DEVICE_SPECIFIER)} {}

Device::~Device()
{
  TYL_ASSERT_NON_NULL(device_);
  alcCloseDevice(reinterpret_cast<ALCdevice*>(device_));
}

}  // namespace tyl::audio::device
