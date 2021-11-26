/**
 * @copyright 2021-present Brian Cairl
 *
 * @file shader.hpp
 */
#pragma once

// Tyl
#include <tyl/graphics/device/shader.hpp>
#include <tyl/ref.hpp>

namespace tyl::graphics
{

using ShaderSource = device::ShaderSource;

/**
 * @brief Shader resource
 */
struct Shader : RefCounted<Shader>, device::Shader
{
  Shader(Shader&& other) = default;
  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source) :
      RefCounted<Shader>{},
      device::Shader{std::move(vertex_source), std::move(fragment_source)}
  {}

  Shader(ShaderSource&& vertex_source, ShaderSource&& fragment_source, ShaderSource&& geometry_source) :
      RefCounted<Shader>{},
      device::Shader{std::move(vertex_source), std::move(fragment_source), std::move(geometry_source)}
  {}

  ~Shader() = default;
};

}  // namespace tyl::graphics
