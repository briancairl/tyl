/**
 * @copyright 2022-present Brian Cairl
 *
 * @file listener.hpp
 */
#pragma once

// Tyl
#include <tyl/device/audio/fwd.hpp>
#include <tyl/device/audio/typedef.hpp>

namespace tyl::audio::device
{

/**
 * @brief Audio listener context
 */
class Listener
{
public:
  explicit Listener(const Device& device);
  ~Listener();

  /**
   * @brief Returns opaque native handle to listener
   */
  constexpr listener_handle_t get_listener_handle() const { return listener_; }

  /**
   * @brief Sets the position of the listener in the current audio field context
   */
  void set_position(const float x, const float y, const float z) const;

  /**
   * @brief Sets the velocity of the listener in the current audio field context
   */
  void set_velocity(const float x, const float y, const float z) const;

  /**
   * @brief Sets the orientation of the listener in the current audio field context
   */
  void
  set_orientation(const float vx, const float vy, const float vz, const float ix, const float iy, const float iz) const;

private:
  listener_handle_t listener_;
};

}  // namespace tyl::audio::device
