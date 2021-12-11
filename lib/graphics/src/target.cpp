/**
 * @copyright 2021-present Brian Cairl
 *
 * @file target.cpp
 */

// C++ Standard Library
#include <cmath>

// Tyl
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/graphics/texture.hpp>

namespace tyl::graphics
{

Target::Target() : viewport_size{0, 0}, active_shader_program{0} { active_texture_units.fill(0); }

bool Target::bind(const Shader& shader_program)
{
  if (active_shader_program != shader_program.get_id())
  {
    active_shader_program = shader_program.get_id();
    shader_program.bind();
    return true;
  }
  return false;
}

bool Target::bind(const Texture& texture, const std::size_t unit)
{
  if (active_texture_units[unit] != texture.get_id())
  {
    active_texture_units[unit] = texture.get_id();
    texture.bind(unit);
    return true;
  }
  return false;
}

bool Target::unbind(const Shader& shader_program)
{
  if (active_shader_program == shader_program.get_id())
  {
    active_shader_program = device::invalid_shader_id;
    shader_program.unbind();
    return true;
  }
  return false;
}

bool Target::unbind(const Texture& texture, const std::size_t unit)
{
  if (active_texture_units[unit] == texture.get_id())
  {
    active_texture_units[unit] = device::invalid_texture_id;
    texture.unbind();
    return true;
  }
  return false;
}

}  // namespace tyl::graphics
