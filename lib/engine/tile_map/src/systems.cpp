/**
 * @copyright 2021-present Brian Cairl
 *
 * @file tile_map.cpp
 */

// C++ Standard Library
#include <algorithm>
#include <cmath>
#include <cstdint>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/components.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/uv_lookup.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/tile_map/initialization.hpp>
#include <tyl/transform.hpp>

namespace tyl::tile_map
{

void render(ecs::Registry& registry, const Mat3f& view_projection_matrix)
{
  // Apply any updates to graphical components of tilemaps
  {
    auto view = registry.view<graphics::tags::updated, graphics::UVLookup, graphics::VertexBuffer, IDMat>();
    for (const auto e : view)
    {
      const auto& [uv_lookup, vertex_buffer, ids] = view.get<graphics::UVLookup, graphics::VertexBuffer, IDMat>(e);

      auto vbptr = vertex_buffer.get_vertex_ptr(3);
      std::transform(
        ids.data(), ids.data() + ids.size(), vbptr.template as<Vec2f>(), [&uv_lookup](const int id) -> Vec2f {
          return uv_lookup[id];
        });

      registry.remove<graphics::tags::updated>(e);
    };
  }

  // Draw tilemaps
  registry.view<Transform, IDMat, graphics::TextureHandle, graphics::ShaderHandle, graphics::VertexBuffer>().each(
    [&view_projection_matrix](
      const Transform& transform,
      const IDMat& tile_map_cells,
      const graphics::TextureHandle& texture,
      const graphics::ShaderHandle& shader,
      const graphics::VertexBuffer& vertex_buffer) {
      TYL_ASSERT_TRUE(shader);
      TYL_ASSERT_TRUE(texture);

      texture.bind(0);
      shader.bind();
      shader.setMat3("uView", view_projection_matrix.data());
      shader.setMat3("uModel", transform.data());
      shader.setMat3("uTextureID", 0);
      vertex_buffer.draw_instanced(tile_map_cells.size());
    });
}

}  // namespace tyl::tile_map
