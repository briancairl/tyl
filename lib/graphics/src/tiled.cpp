/**
 * @copyright 2022-present Brian Cairl
 *
 * @file tiled.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/tiled.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

void attach_tiled_batch_renderer_shader(ecs::registry& registry, const ecs::entity id)
{
  // clang-format off
  registry.emplace<Shader>(
    id,
    ShaderSource::vertex(
      R"VertexShader(

      // Vertex-buffer layout
      layout (location = 0) in vec4 layout_PositionRect;  // { corner, extents }
      layout (location = 1) in vec4 layout_TexCoordRect;  // { corner, extents }

      // Texture UV coordinate output
      out vec4 vshader_TexCoordRect;

      void main()
      {
        gl_Position = layout_PositionRect;
        vshader_TexCoordRect = layout_TexCoordRect;
      }

      )VertexShader"
    ),
    ShaderSource::fragment(
      R"FragmentShader(

      // Fragment color
      out vec4 FragColor;

      // Texture UV coordinate from vertex shader
      in vec2 gshader_TexCoord;

      // Texture sampling unit uniform
      uniform sampler2D u_TextureID;

      void main()
      {
        FragColor = texture(u_TextureID, gshader_TexCoord);
      }

      )FragmentShader"
    ),
    ShaderSource::geometry(
      R"GeometryShader(
      layout(points) in;
      layout(triangle_strip, max_vertices = 4) out;

      // View-projection matrix uniform
      uniform mat3 u_ViewProjection;

      // Texture UV coordinate from vertex shader
      in vec4[] vshader_TexCoordRect;

      // Texture UV coordinates to fragement shader
      out vec2 gshader_TexCoord;

      void main()
      {
        // Left corner of the tile
        vec2 corner = vec2(gl_in[0].gl_Position[0], gl_in[0].gl_Position[1]);

        // Size of the tile along x/y
        vec2 extents = vec2(gl_in[0].gl_Position[2], gl_in[0].gl_Position[3]);

        // Left corner of the tile
        vec2 uv_corner = vec2(vshader_TexCoordRect[0][0], vshader_TexCoordRect[0][1]);

        // Size of the tile along x/y
        vec2 uv_extents = vec2(vshader_TexCoordRect[0][2], vshader_TexCoordRect[0][3]);

        // 1:bottom-left
        gl_Position = vec4(u_ViewProjection * vec3(corner, 1), 1);
        gshader_TexCoord = vec2(uv_corner[0], uv_corner[1] + uv_extents[1]);
        EmitVertex();

        // 2:bottom-right
        gl_Position = vec4(u_ViewProjection * vec3(corner + vec2(extents[0],  0.0), 1), 1);
        gshader_TexCoord = vec2(uv_corner[0] + uv_extents[0], uv_corner[1] + uv_extents[1]);
        EmitVertex();

        // 3:top-left
        gl_Position = vec4(u_ViewProjection * vec3(corner + vec2(0.0, extents[1]), 1), 1);
        gshader_TexCoord = vec2(uv_corner[0], uv_corner[1]);
        EmitVertex();

        // 4:top-right
        gl_Position = vec4(u_ViewProjection * vec3(corner + vec2(extents[0],  extents[1]), 1), 1);
        gshader_TexCoord = vec2(uv_corner[0] + uv_extents[0], uv_corner[1]);
        EmitVertex();

        EndPrimitive();
      }

      )GeometryShader"
    )
  );
}

static constexpr std::size_t TILED_OFFSET_POSITION_INDEX = 0;
static constexpr std::size_t TILED_OFFSET_TEXCOORD_INDEX = 1;

void attach_tiled_batch_renderer_vertex_buffer(ecs::registry& registry, const ecs::entity e, const std::size_t tile_count)
{
  // clang-format off
  registry.emplace<VertexBuffer>(
    e,
    [tile_count] {
      VertexBuffer vb{{
                        VertexBuffer::Attribute{device::typecode<float>(), 4, tile_count},  // position [corner, extents]
                        VertexBuffer::Attribute{device::typecode<float>(), 4, tile_count},  // texcoord [corner, extents]
                      },
                      VertexBuffer::BufferMode::DYNAMIC};
      return vb;
    }());
  // clang-format on
}

}  // namespace anonymous

ecs::entity create_tiled_batch_renderer(ecs::registry& registry, const std::size_t max_tile_count)
{
  const ecs::entity entity_id = registry.create();
  attach_tiled_batch_renderer(registry, entity_id, max_tile_count);
  return entity_id;
}

void attach_tiled_batch_renderer(ecs::registry& registry, const ecs::entity entity_id, const std::size_t max_tile_count)
{
  attach_tiled_batch_renderer_vertex_buffer(registry, entity_id, max_tile_count);
  attach_tiled_batch_renderer_shader(registry, entity_id);
  registry.emplace<TiledBatchRenderProperties>(entity_id, max_tile_count);
}

void draw_tiles(ecs::registry& registry, Target& render_target, const duration dt)
{
  using W_Texture = ecs::Ref<Texture>;
  using W_TileUVLookup = ecs::Ref<TileUVLookup>;

  registry.view<ViewProjection, ViewportRect>().each([&](const auto& view_projection, const auto& view_rect) {
    registry.view<TiledBatchRenderProperties, VertexBuffer, Shader>().each(
      [&](const auto& render_props, const auto& vertex_buffer, const auto& shader) {
        // Set shader program if its not already active
        render_target.bind(shader);

        // Set view projection matrix
        shader.setMat3("u_ViewProjection", reinterpret_cast<const float*>(std::addressof(view_projection)));

        // Eliminate renderables which aren't in view
        {
          auto tile_view = registry.template view<TiledRegion, Rect2D>();
          for (const ecs::entity tile_id : tile_view)
          {
            if (view_rect.intersects(tile_view.get<Rect2D>(tile_id)))
            {
              registry.emplace<TiledRenderingEnabled>(tile_id);
            }
          }
        }

        // Buffer tile data (position, uv)
        std::size_t tile_count = 0;

        {
          auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(TILED_OFFSET_POSITION_INDEX);
          auto position_data = vb_buffer_ptr.template as<Vec4f>();
          auto texcoord_data = position_data + render_props.max_tile_count;
          auto tile_view =
            registry.template view<TiledRenderingEnabled, Rect2D, TiledRegion, W_Texture, W_TileUVLookup>();
          for (const auto tile_id : tile_view)
          {
            static constexpr std::size_t texture_unit = 0;

            // Stop buffering tiles if we hit the max tile count
            if (tile_count + TiledRegion::N > render_props.max_tile_count)
            {
              break;
            }
            // Set active texture unit if its not already active
            else if (const auto& texture = (*tile_view.template get<W_Texture>(tile_id));
                     render_target.bind(texture, texture_unit))
            {
              shader.setInt("u_TextureID", texture_unit);
            }

            // Set tile position info
            {
              const auto& rect = tile_view.template get<Rect2D>(tile_id);
              const float tile_size_x{rect.extents().x() / TiledRegion::S};
              const float tile_size_y{rect.extents().y() / TiledRegion::S};

              for (int i = 0; i < TiledRegion::S; ++i)
              {
                for (int j = 0; j < TiledRegion::S; ++j, ++position_data)
                {
                  position_data->template head<2>() = rect.min() + Vec2f{i * tile_size_x, j * tile_size_y};
                  position_data->template tail<2>() << tile_size_x, tile_size_y;
                }
              }
            }

            // Set tile tile info
            {
              const TileUVLookup& uv_lookup = (*tile_view.template get<W_TileUVLookup>(tile_id));
              const TiledRegion& tile = tile_view.template get<TiledRegion>(tile_id);

              for (int i = 0; i < TiledRegion::S; ++i)
              {
                for (int j = 0; j < TiledRegion::S; ++j, ++texcoord_data)
                {
                  (*texcoord_data) = uv_lookup[tile.ids[i * TiledRegion::S + j]];
                }
              }
            }

            // Increment tile count to track number of buffered tile data
            tile_count += TiledRegion::N;
          }
        }

        // Draw all the tiles that we buffered
        vertex_buffer.draw(tile_count, graphics::VertexBuffer::DrawMode::POINTS);
      });

    for (const auto id : registry.view<TiledRenderingEnabled>())
    {
      registry.remove<TiledRenderingEnabled>(id);
    }
  });
}

ecs::entity create_tiled(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tile_rect)
{
  const ecs::entity entity_id = registry.create();
  attach_tiled(registry, entity_id, uv_lookup, tile_rect);
  return entity_id;
}

void attach_tiled(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& tile_rect)
{
  TYL_ASSERT_FALSE(registry.has<TiledRegion>(entity_id));
  registry.emplace<ecs::Ref<Texture>>(entity_id, ecs::ref<ecs::Ref<Texture>>(uv_lookup));
  registry.emplace<ecs::Ref<TileUVLookup>>(entity_id, ecs::ref<TileUVLookup>(uv_lookup));
  registry.emplace<Rect2D>(entity_id, tile_rect);
  registry.emplace<TiledRegion>(entity_id).ids.fill(0);
}

}  // namespace tyl::graphics
