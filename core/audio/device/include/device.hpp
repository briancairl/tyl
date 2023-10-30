/**
 * @copyright 2022-present Brian Cairl
 *
 * @file device.hpp
 */
#pragma once

// Tyl
#include <tyl/audio/device/typedef.hpp>

namespace tyl::audio::device
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
   * @brief Returns opaque native handle to device
   */
  constexpr device_handle_t get_device_handle() const { return device_; }

private:
  /// Handle to device
  device_handle_t device_;
};

}  // namespace tyl::audio::device
