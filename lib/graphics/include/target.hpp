/**
 * @copyright 2021-present Brian Cairl
 *
 * @file target.hpp
 */
#pragma once

// C++ Standard Library
#include <array>

// Tyl
#include <tyl/graphics/device/constants.hpp>
#include <tyl/graphics/device/typedef.hpp>
#include <tyl/graphics/fwd.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{

/**
 * @brief Describes current render target state
 */
class Target
{
public:
  /// Size of the view port area
  Size2i viewport_size;

  Target();

  /**
   * @brief Activates a shader program
   *
   * @retval true  if shader was newly bound to graphics device
   * @retval false  if shader was already bound
   */
  bool bind(const Shader& shader_program);

  /**
   * @brief Activates a texture
   *
   * @retval true  if texture was newly bound to graphics device
   * @retval false  if texture was already bound
   */
  bool bind(const Texture& texture, const std::size_t unit = 0UL);

  /**
   * @brief Deactivates a shader program
   *
   * @retval true  if shader was newly bound to graphics device
   * @retval false  if shader was already bound
   */
  bool unbind(const Shader& shader_program);

  /**
   * @brief Deactivates a texture
   *
   * @retval true  if texture was newly bound to graphics device
   * @retval false  if texture was already bound
   */
  bool unbind(const Texture& texture, const std::size_t unit = 0UL);

private:
  /// Shader program currently in use
  device::shader_id_t active_shader_program = 0;

  /// Texture units currently in use
  std::array<device::texture_id_t, device::texture_unit_count> active_texture_units;
};

}  // namespace tyl::graphics
