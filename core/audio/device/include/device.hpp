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

  Device(Device&& other);

  Device(const Device& other) = delete;

  Device();

  ~Device();

  /**
   * @brief Sets a device active
   */
  bool enable() const;

  /**
   * @brief Sets a device inactive
   */
  bool disable() const;

  /**
   * @brief Returns opaque native handle to device
   */
  constexpr device_handle_t get_device_handle() const { return device_; }

  /**
   * @brief Returns true if Device is valid
   */
  constexpr bool is_valid() const { return device_ != kInvalidDeviceHandle; }

private:
  /// Handle to device
  device_handle_t device_;
  /// Audio context
  context_handle_t context_;
};

}  // namespace tyl::audio::device
