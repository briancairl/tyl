/**
 * @copyright 2021-present Brian Cairl
 *
 * @file shader.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/fwd.hpp>

namespace tyl::graphics
{

using ShaderSource = device::ShaderSource;

/**
 * @brief Shader resource
 */
class Shader : public ecs::make_handle_from_this<Shader>, public device::Shader
{
public:
  Shader(Shader&& other) = default;
  Shader& operator=(Shader&&) = default;

  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source) :
      ecs::make_handle_from_this<Shader>{},
      device::Shader{std::move(vertex_source), std::move(fragment_source)}
  {}

  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source, ShaderSource&& geometry_source) :
      ecs::make_handle_from_this<Shader>{},
      device::Shader{std::move(vertex_source), std::move(fragment_source), std::move(geometry_source)}
  {}

  ~Shader() = default;

private:
  using device::Shader::bind;

  friend class Target;
};

}  // namespace tyl::graphics
