/**
 * @copyright 2021-present Brian Cairl
 *
 * @file initialization.cpp
 */

// C++ Standard Library
#include <cstdint>

// Tyl
#include <tyl/graphics/components.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/uv_lookup.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/tile_map/components.hpp>
#include <tyl/tile_map/initialization.hpp>

namespace tyl::tile_map
{
namespace  // anonymous
{

void attach_render_data(
  ecs::Registry& registry,
  const ecs::Entity entity,
  const ecs::Entity texture_entity,
  const ecs::Entity shader_entity)
{
  using namespace graphics;

  const auto& [texture, texture_size] = registry.get<Texture, TotalSizePx>(texture_entity);

  registry.emplace<graphics::tags::updated>(entity);
  registry.emplace<UVLookup>(entity, registry.get<TileSizePx>(entity), texture_size);
  registry.emplace<TextureHandle>(entity, texture);
  registry.emplace<ShaderHandle>(entity, registry.get<Shader>(shader_entity));
  registry.emplace<VertexBuffer>(
    entity,
    [tile_size = registry.get<TileSizePx>(entity),
     tile_size_uv = registry.get<UVLookup>(entity).tile_size_uv(),
     tile_map_rows = registry.get<IDMat>(entity).rows(),
     tile_map_cols = registry.get<IDMat>(entity).cols()] {
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

}  // namespace anonymous

ecs::Entity create_default_shader(ecs::Registry& registry)
{
  using namespace graphics;

  const ecs::Entity entity = registry.create();

  // clang-format off
  registry.emplace<graphics::Shader>(
    entity,
    graphics::ShaderSource::vertex(
      R"VertexShader(

      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTexCoord;
      layout (location = 2) in vec2 aPosOffset;
      layout (location = 3) in vec2 aTexCoordOffset;

      uniform mat3 uView;
      uniform mat3 uModel;

      out vec2 vsTexCoord;

      void main()
      {
        gl_Position =  vec4(uView * uModel * vec3(aPos + aPosOffset, 1), 1);
        vsTexCoord = aTexCoord + aTexCoordOffset;
      }

      )VertexShader"
    ),
    graphics::ShaderSource::fragment(
      R"FragmentShader(

      out vec4 FragColor;

      in vec2 vsTexCoord;

      uniform sampler2D uTextureID;

      void main()
      {
        FragColor = texture(uTextureID, vsTexCoord);
      }

      )FragmentShader"
    )
  );
  // clang-format on
  return entity;
}

ecs::Entity create(ecs::Registry& registry, const Options& options)
{
  const ecs::Entity entity = registry.create();
  attach(registry, entity, options);
  return entity;
}

void attach(ecs::Registry& registry, const ecs::Entity entity, const Options& options)
{
  registry.emplace<IDMat>(entity, options.dimensions.x(), options.dimensions.y()).fill(options.initial_tile_id);
  registry.emplace<TileSizePx>(entity, options.tile_size);
  registry.emplace<Transform>(entity, options.transform);
  attach_render_data(registry, entity, options.altas_texture_entity, options.shader_entity);
}

}  // namespace tyl::tile_map
