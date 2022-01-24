/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_renderer.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/sprite_renderer.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/tiled.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/vec.hpp>

#include <iostream>

namespace tyl::graphics
{
namespace  // anonymous
{

// Internal flagging component used to enable rendering to the current viewport
struct ViewportRenderingEnabled
{};

void attach_sprite_renderer_shader(ecs::registry& registry, const ecs::entity id)
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
  // clang-format on
}

static constexpr std::size_t SPRITE_RECT_INDEX = 0;
static constexpr std::size_t SPRITE_RECT_UV_INDEX = 1;

void attach_sprite_renderer_vertex_buffer(ecs::registry& registry, const ecs::entity e, const std::size_t sprite_count)
{
  // clang-format off
  registry.emplace<VertexBuffer>(
    e,
    [sprite_count] {
      VertexBuffer vb{{
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // position [corner, extents]
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // texcoord [corner, extents]
                      },
                      VertexBuffer::BufferMode::DYNAMIC};
      return vb;
    }());
  // clang-format on
}

void cull_sprites_for_viewport(ecs::registry& registry, const ViewportRect& viewport_rect)
{
  auto sprite_view = registry.view<RenderingEnabled, Rect2D>();
  for (const ecs::entity sprite_id : sprite_view)
  {
    if (viewport_rect.intersects(sprite_view.get<Rect2D>(sprite_id)))
    {
      registry.emplace<ViewportRenderingEnabled>(sprite_id);
    }
  }
}

void reset_sprites_for_viewport(ecs::registry& registry)
{
  auto sprite_view = registry.view<ViewportRenderingEnabled, Rect2D>();
  for (const ecs::entity sprite_id : sprite_view)
  {
    registry.remove<ViewportRenderingEnabled>(sprite_id);
  }
}

void draw_dynamic_sprites(
  Target& render_target,
  ecs::registry& registry,
  SpriteRendererProperties& render_props,
  const VertexBuffer& vertex_buffer,
  const Shader& shader)
{
  using W_Texture = ecs::Ref<Texture>;
  using W_TileUVLookup = ecs::Ref<TileUVLookup>;

  {
    auto vbo = vertex_buffer.get_vertex_ptr(SPRITE_RECT_INDEX);
    auto* position_ptr = vbo.as<Vec4f>() + render_props.current_sprite_count;
    auto* texcoord_ptr = vbo.as<Vec4f>() + render_props.current_sprite_count + render_props.max_sprite_count;
    auto sprite_view =
      registry.template view<ViewportRenderingEnabled, Rect2D, SpriteTileID, W_Texture, W_TileUVLookup>();
    for (const auto sprite_id : sprite_view)
    {
      static constexpr std::size_t DRAW_TEXTURE_UNIT = 0;

      // Stop buffering sprites if we hit the max sprite count
      if (render_props.current_sprite_count > render_props.max_sprite_count)
      {
        break;
      }
      // Set active texture unit if its not already active
      else if (const auto& texture = (*sprite_view.get<W_Texture>(sprite_id));
               render_target.bind(texture, DRAW_TEXTURE_UNIT))
      {
        shader.setInt("u_TextureID", DRAW_TEXTURE_UNIT);
      }

      // Set sprite position info
      {
        const auto& rect = sprite_view.get<Rect2D>(sprite_id);
        position_ptr->head<2>() = rect.min();
        position_ptr->tail<2>() = rect.extents();
      }

      // Set sprite tile info
      {
        const auto& uv_lookup = (*sprite_view.get<W_TileUVLookup>(sprite_id));
        const auto& tile = sprite_view.get<SpriteTileID>(sprite_id);
        (*texcoord_ptr) = uv_lookup[tile.id];
      }

      // Increment buffer pointers
      ++render_props.current_sprite_count;
      ++position_ptr;
      ++texcoord_ptr;
    }
  }

  if (render_props.current_sprite_count > 0)
  {
    vertex_buffer.draw(render_props.current_sprite_count, graphics::VertexBuffer::DrawMode::POINTS);
    render_props.current_sprite_count = 0UL;
  }
}

void reset_dynamic_sprites(ecs::registry& registry)
{
  for (const auto sprite_id : registry.view<RenderingEnabled, SpriteTileID>())
  {
    registry.remove<RenderingEnabled>(sprite_id);
  }
}

void draw_static_sprites(
  Target& render_target,
  ecs::registry& registry,
  SpriteRendererProperties& render_props,
  const VertexBuffer& vertex_buffer,
  const Shader& shader)
{
  using W_Texture = ecs::Ref<Texture>;
  using W_TileUVLookup = ecs::Ref<TileUVLookup>;

  {
    auto vbo = vertex_buffer.get_vertex_ptr(SPRITE_RECT_INDEX);
    auto* position_ptr = vbo.as<Vec4f>() + render_props.current_sprite_count;
    auto* texcoord_ptr = vbo.as<Vec4f>() + render_props.current_sprite_count + render_props.max_sprite_count;
    auto tile_view = registry.view<ViewportRenderingEnabled, Rect2D, TiledRegion, W_Texture, W_TileUVLookup>();

    for (const auto tiled_id : tile_view)
    {
      static constexpr std::size_t DRAW_TEXTURE_UNIT = 1;

      // Stop buffering tiles if we will exceed the max sprite count
      if (render_props.current_sprite_count + TiledRegion::N > render_props.max_sprite_count)
      {
        break;
      }
      // Set active texture unit if its not already active
      else if (const auto& texture = (*tile_view.get<W_Texture>(tiled_id));
               render_target.bind(texture, DRAW_TEXTURE_UNIT))
      {
        shader.setInt("u_TextureID", DRAW_TEXTURE_UNIT);
      }

      // Set tile position info
      {
        const auto& rect = tile_view.get<Rect2D>(tiled_id);
        const float tile_size_x{rect.extents().x() / TiledRegion::S};
        const float tile_size_y{rect.extents().y() / TiledRegion::S};

        for (int i = 0; i < TiledRegion::S; ++i)
        {
          for (int j = 0; j < TiledRegion::S; ++j, ++position_ptr)
          {
            position_ptr->head<2>() = rect.min() + Vec2f{j * tile_size_x, i * tile_size_y};
            position_ptr->tail<2>() << tile_size_x, tile_size_y;
          }
        }
      }

      // Set tile tile info
      {
        const auto& uv_lookup = (*tile_view.get<W_TileUVLookup>(tiled_id));
        const auto& tile = tile_view.get<TiledRegion>(tiled_id);

        for (int i = 0; i < TiledRegion::S; ++i)
        {
          for (int j = 0; j < TiledRegion::S; ++j, ++texcoord_ptr)
          {
            const auto tile_id = tile.ids[i * TiledRegion::S + j];
            (*texcoord_ptr) = uv_lookup[tile_id];
          }
        }
      }

      // Increment tile count to track number of buffered tile data
      render_props.current_sprite_count += TiledRegion::N;
    }
  }

  if (render_props.current_sprite_count > 0)
  {
    vertex_buffer.draw(render_props.current_sprite_count, graphics::VertexBuffer::DrawMode::POINTS);
    render_props.current_sprite_count = 0UL;
  }
}

}  // namespace anonymous

ecs::entity create_sprite_renderer(ecs::registry& registry, const std::size_t max_sprite_count)
{
  const ecs::entity entity_id = registry.create();
  attach_sprite_renderer(registry, entity_id, max_sprite_count);
  return entity_id;
}

void attach_sprite_renderer(ecs::registry& registry, const ecs::entity entity_id, const std::size_t max_sprite_count)
{
  attach_sprite_renderer_vertex_buffer(registry, entity_id, max_sprite_count);
  attach_sprite_renderer_shader(registry, entity_id);
  registry.emplace<SpriteRendererProperties>(entity_id, max_sprite_count, 0UL);
}

void draw_sprites(ecs::registry& registry, Target& render_target, const duration dt)
{
  registry.view<ViewProjection, ViewportRect>().each(
    [&](const ViewProjection& view_projection, const ViewportRect& viewport_rect) {
      // Enable rendering for those sprite-like entities which are currently visible
      cull_sprites_for_viewport(registry, viewport_rect);

      // Setup shader program
      registry.view<SpriteRendererProperties, Shader>().each(
        [&render_target, &view_projection](const SpriteRendererProperties& render_props, const Shader& shader) {
          // Set shader program if its not already active
          render_target.bind(shader);

          // Set view projection matrix
          shader.setMat3("u_ViewProjection", reinterpret_cast<const float*>(std::addressof(view_projection)));
        });

      // Draw semi-static tiled regions
      registry.view<SpriteRendererProperties, VertexBuffer, Shader>().each(
        [&](SpriteRendererProperties& render_props, const VertexBuffer& vertex_buffer, const Shader& shader) {
          draw_static_sprites(render_target, registry, render_props, vertex_buffer, shader);
        });

      // Draw dynamic sprites
      // registry.view<SpriteRendererProperties, VertexBuffer, Shader>().each(
      //   [&](SpriteRendererProperties& render_props, const VertexBuffer& vertex_buffer, const Shader& shader) {
      //     draw_dynamic_sprites(render_target, registry, render_props, vertex_buffer, shader);
      //   });

      // Disable rendering for visible sprites
      reset_sprites_for_viewport(registry);
    });

  // Disable rendering for dynamic sprites
  reset_dynamic_sprites(registry);
}

}  // namespace tyl::graphics
