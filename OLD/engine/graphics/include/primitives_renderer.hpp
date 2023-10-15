/**
 * @copyright 2023-present Brian Cairl
 *
 * @file primitives_renderer.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Entt
#include <entt/entt.hpp>

// Tyl
#include <tyl/engine/graphics/camera.hpp>
#include <tyl/utility/expected.hpp>

namespace tyl::engine::graphics
{

class PrimitivesRenderer
{
public:
  enum class ErrorCode
  {
    SHADER_CREATION_FAILURE_VERTEX,
    SHADER_CREATION_FAILURE_FRAGMENT,
    SHADER_LINKAGE_FAILURE
  };

  struct Settings
  {
    std::size_t max_vertex_count;
  };

  [[nodiscard]] static tyl::expected<PrimitivesRenderer, ErrorCode> create(const Settings& settings);

  PrimitivesRenderer(PrimitivesRenderer&&) = default;

  ~PrimitivesRenderer();

  void draw(const CameraMatrix2D& camera_matrix, const entt::registry& reg);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
  explicit PrimitivesRenderer(std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine::graphics
