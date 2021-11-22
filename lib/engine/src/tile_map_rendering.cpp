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
#include <tyl/common/assert.hpp>
#include <tyl/engine/camera.hpp>
#include <tyl/engine/geometry.hpp>
#include <tyl/engine/state.hpp>
#include <tyl/engine/tile_map.hpp>
#include <tyl/engine/tile_map_rendering.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/vertex_buffer.hpp>

namespace tyl::engine
{

TileAtlasUVLookup::TileAtlasUVLookup(const TileSizePx& tile_size, const TileAtlasSizePx& tile_altas_size)
{
  const int quantized_tile_rows = tile_altas_size.x() / tile_size.x();
  const int quantized_tile_cols = tile_altas_size.y() / tile_size.y();

  uv_tile_size_.x() = 1.f / quantized_tile_cols;
  uv_tile_size_.y() = 1.f / quantized_tile_rows;

  uv_offsets_.reserve(quantized_tile_rows * quantized_tile_cols);
  for (int x = 0; x < quantized_tile_rows; ++x)
  {
    for (int y = 0; y < quantized_tile_cols; ++y)
    {
      uv_offsets_.emplace_back(x * uv_tile_size_.x(), y * uv_tile_size_.y());
    }
  }
}

void add_tile_map_render_data(
  ECSRegistry& registry,
  const Entity entity,
  const TileAtlasSizePx& tile_altas_size,
  const graphics::TextureHandle& tile_atlas_tex,
  const graphics::ShaderHandle& tile_map_shader)
{
  using namespace graphics;

  registry.emplace<TileAtlasUVLookup>(entity, registry.get<TileSizePx>(entity), tile_altas_size);
  registry.emplace<TextureHandle>(entity, tile_atlas_tex);
  registry.emplace<ShaderHandle>(entity, tile_map_shader);
  registry.emplace<VertexBuffer>(
    entity,
    [tile_size = registry.get<TileSizePx>(entity),
     tile_size_uv = registry.get<TileAtlasUVLookup>(entity).tile_size_uv(),
     tile_map_rows = registry.get<TileMapGrid>(entity).rows(),
     tile_map_cols = registry.get<TileMapGrid>(entity).cols()] {
      const std::size_t n_cells = static_cast<std::size_t>(tile_map_rows * tile_map_cols);

      VertexBuffer vb{6UL,
                      {
                        VertexAttributeDescriptor{TypeCode::Float32, 2, 4, 0},  // quad vertex position
                        VertexAttributeDescriptor{TypeCode::Float32, 2, 4, 0},  // quad texcoords
                        VertexAttributeDescriptor{TypeCode::Float32, 2, n_cells, 1},  // vertex offset
                        VertexAttributeDescriptor{TypeCode::Float32, 2, n_cells, 1},  // texcoord offset
                      },
                      VertexBuffer::BufferMode::DYNAMIC};

      {
        const unsigned indices[] = {0, 1, 2, 2, 3, 0};
        vb.set_index_data(indices);
      }

      {
        const Vec2f points[] = {
          Vec2f(0, 0),
          Vec2f(tile_size.x(), 0),
          Vec2f(tile_size.x(), tile_size.y()),
          Vec2f(0, tile_size.y()),
        };
        vb.set_vertex_data(0, reinterpret_cast<const float*>(points));
      }

      {
        const Vec2f texcoords[] = {
          Vec2f{0.f, tile_size_uv.y()},
          Vec2f{tile_size_uv.x(), tile_size_uv.y()},
          Vec2f{tile_size_uv.x(), 0.f},
          Vec2f{0.f, 0.f},
        };
        vb.set_vertex_data(1, reinterpret_cast<const float*>(texcoords));
      }

      {
        auto buffer_ptr = vb.get_vertex_ptr(2);
        Vec2f* vec_ptr = buffer_ptr.as<Vec2f>();
        for (int x = 0; x < tile_map_rows; ++x)
        {
          for (int y = 0; y < tile_map_cols; ++y, ++vec_ptr)
          {
            vec_ptr->x() = tile_size.x() * x;
            vec_ptr->y() = tile_size.y() * y;
          }
        }
      }

      {
        auto buffer_ptr = vb.get_vertex_ptr(3);
        Vec2f* vec_ptr = buffer_ptr.as<Vec2f>();
        for (int x = 0; x < tile_map_rows; ++x)
        {
          for (int y = 0; y < tile_map_cols; ++y, ++vec_ptr)
          {
            (*vec_ptr) = Vec2f::Zero();
          }
        }
      }

      return vb;
    }());
}

void render_tile_maps(ECSRegistry& registry, const ViewProjectionMatrix& view_projection_matrix)
{
  using namespace graphics;

  const auto view = registry.view<
    const Transform,
    const TileAtlasUVLookup,
    const TileMapGrid,
    const TextureHandle,
    const ShaderHandle,
    const VertexBuffer,
    UpdateFlags>();

  view.each([&view_projection_matrix](
              const Transform& transform,
              const TileAtlasUVLookup& tile_atlas_offset,
              const TileMapGrid& tile_map_cells,
              const TextureHandle& texture,
              const ShaderHandle& shader,
              const VertexBuffer& vertex_buffer,
              UpdateFlags& update_flags) {
    TYL_ASSERT_TRUE(shader);
    TYL_ASSERT_TRUE(texture);

    // Update tile UV offset if there are render changes to be made
    if (update_flags.has_render_changes)
    {
      auto vbptr = vertex_buffer.get_vertex_ptr(3);

      std::transform(
        tile_map_cells.data(),
        tile_map_cells.data() + tile_map_cells.size(),
        vbptr.as<Vec2f>(),
        [&tile_atlas_offset](const unsigned cell_index) -> Vec2f { return tile_atlas_offset[cell_index]; });

      update_flags.has_render_changes = false;
    }

    texture.bind(0);
    shader.bind();
    const tyl::Mat3f mvp{view_projection_matrix * transform};
    shader.setMat3("uModelView", mvp.data());
    shader.setMat3("uTextureID", 0);
    vertex_buffer.draw_instanced(tile_map_cells.size());
  });
}

}  // namespace tyl::engine
