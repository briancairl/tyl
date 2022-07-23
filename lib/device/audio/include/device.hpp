/**
 * @copyright 2022-present Brian Cairl
 *
 * @file device.hpp
 */
#pragma once

// Tyl
#include <tyl/device/audio/typedef.hpp>

namespace tyl::device::audio
{

/**
 * @brief Audio device wrapper
 */
class Device
{
public:
  explicit Device(const char* device_name);
  Device();
  ~Device();

  /**
   * @brief Returns name of the audio device according to the host system
   */
  constexpr const char* get_device_name() const { return device_name_; }

  /**
   * @brief Returns opaque native handle to device
   */
  constexpr device_handle_t get_device_handle() const { return device_; }

private:
  /// Name of the device according to system
  const char* device_name_;

  /// Handle to device
  device_handle_t device_;
};

}  // namespace tyl::device::audio
