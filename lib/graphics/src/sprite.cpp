/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite_sequence.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

void attach_sprite_batch_renderer_shader(ecs::registry& registry, const ecs::entity id)
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

void attach_sprite_batch_renderer_vertex_buffer(
  ecs::registry& registry,
  const ecs::entity e,
  const std::size_t sprite_count)
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

}  // namespace anonymous

ecs::entity create_sprite_batch_renderer(ecs::registry& registry, const std::size_t max_sprite_count)
{
  const ecs::entity entity_id = registry.create();
  attach_sprite_batch_renderer(registry, entity_id, max_sprite_count);
  return entity_id;
}

void attach_sprite_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_sprite_count)
{
  attach_sprite_batch_renderer_vertex_buffer(registry, entity_id, max_sprite_count);
  attach_sprite_batch_renderer_shader(registry, entity_id);
  registry.emplace<SpriteBatchRenderProperties>(entity_id, max_sprite_count);
}

void draw_sprites(ecs::registry& registry, Target& render_target, const duration dt)
{
  using W_Texture = ecs::Ref<Texture>;
  using W_TileUVLookup = ecs::Ref<TileUVLookup>;

  registry.view<ViewProjection, ViewportRect>().each([&](const auto& view_projection, const auto& view_rect) {
    registry.view<SpriteBatchRenderProperties, VertexBuffer, Shader>().each(
      [&](const auto& render_props, const auto& vertex_buffer, const auto& shader) {
        // Set shader program if its not already active
        render_target.bind(shader);

        // Set view projection matrix
        shader.setMat3("u_ViewProjection", reinterpret_cast<const float*>(std::addressof(view_projection)));

        // Eliminate renderables which aren't in view
        {
          auto sprite_view = registry.template view<SpriteRenderingEnabled, Rect2D>();
          for (const ecs::entity sprite_id : sprite_view)
          {
            if (!view_rect.intersects(sprite_view.get<Rect2D>(sprite_id)))
            {
              registry.remove<SpriteRenderingEnabled>(sprite_id);
            }
          }
        }

        // Buffer sprite data (position, uv)
        std::size_t sprite_count = 0;
        {
          auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(SPRITE_RECT_INDEX);
          auto position_data = vb_buffer_ptr.template as<Vec4f>();
          auto texcoord_data = position_data + render_props.max_sprite_count;
          auto sprite_view =
            registry.template view<SpriteRenderingEnabled, Rect2D, SpriteTileID, W_Texture, W_TileUVLookup>();
          for (const auto sprite_id : sprite_view)
          {
            static constexpr std::size_t texture_unit = 0;

            // Stop buffering sprites if we hit the max sprite count
            if (sprite_count > render_props.max_sprite_count)
            {
              break;
            }
            // Set active texture unit if its not already active
            else if (const auto& texture = (*sprite_view.template get<W_Texture>(sprite_id));
                     render_target.bind(texture, texture_unit))
            {
              shader.setInt("u_TextureID", texture_unit);
            }

            // Set sprite position info
            {
              const auto& rect = sprite_view.template get<Rect2D>(sprite_id);
              position_data->template head<2>() = rect.min();
              position_data->template tail<2>() = rect.extents();
            }

            // Set sprite tile info
            {
              const TileUVLookup& uv_lookup = (*sprite_view.template get<W_TileUVLookup>(sprite_id));
              const SpriteTileID& tile = sprite_view.template get<SpriteTileID>(sprite_id);
              (*texcoord_data) = uv_lookup[tile.id];
            }

            // Increment buffer pointers
            ++sprite_count;
            ++position_data;
            ++texcoord_data;
          }
        }

        // Draw all the sprites that we buffered
        vertex_buffer.draw(sprite_count, graphics::VertexBuffer::DrawMode::POINTS);
      });

    // Update looped dynamic sprite sequences
    registry.view<SpriteRenderingEnabled, SpriteSequenceLooped, SpriteSequence, SpriteTileID, duration>().each(
      [dt](SpriteSequence& sequence, SpriteTileID& tile, const duration& update_period) {
        sequence.period_since_last_update += dt;
        if (sequence.period_since_last_update < update_period)
        {
          return;
        }
        else if (tile.id == sequence.stop_id)
        {
          tile.id = sequence.start_id;
        }
        else
        {
          ++tile.id;
        }
        sequence.period_since_last_update = duration::zero();
      });

    // Update one-shot dynamic sprite sequences
    registry.view<SpriteRenderingEnabled, SpriteSequenceOneShot, SpriteSequence, SpriteTileID, duration>().each(
      [dt](SpriteSequence& sequence, SpriteTileID& tile, const duration& update_period) {
        sequence.period_since_last_update += dt;
        if (sequence.period_since_last_update > update_period and tile.id < sequence.stop_id)
        {
          ++tile.id;
          sequence.period_since_last_update = duration::zero();
        }
      });

    for (const auto id : registry.view<SpriteRenderingEnabled>())
    {
      registry.remove<SpriteRenderingEnabled>(id);
    }
  });
}

ecs::entity create_sprite(
  ecs::registry& registry,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect)
{
  const ecs::entity entity_id = registry.create();
  attach_sprite(registry, entity_id, uv_lookup, sprite_rect);
  return entity_id;
}

void attach_sprite(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const ecs::Ref<TileUVLookup, ecs::Ref<Texture>> uv_lookup,
  const Rect2D& sprite_rect)
{
  TYL_ASSERT_FALSE(registry.has<SpriteTileID>(entity_id));
  registry.emplace<ecs::Ref<Texture>>(entity_id, ecs::ref<ecs::Ref<Texture>>(uv_lookup));
  registry.emplace<ecs::Ref<TileUVLookup>>(entity_id, ecs::ref<TileUVLookup>(uv_lookup));
  registry.emplace<Rect2D>(entity_id, sprite_rect);
  registry.emplace<SpriteTileID>(entity_id, 0UL);
}

void attach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id, const float rate, const bool looped)
{
  TYL_ASSERT_FALSE(registry.has<SpriteSequence>(entity_id));
  TYL_ASSERT_TRUE((registry.has<SpriteTileID, ecs::Ref<TileUVLookup>>(entity_id)));
  TYL_ASSERT_GT(rate, 0.0f);

  {
    auto [tile, tile_uv] = registry.get<SpriteTileID, ecs::Ref<TileUVLookup>>(entity_id);
    tile.id = 0;
    registry.emplace<SpriteSequence>(entity_id, tile.id, (*tile_uv).tile_count() - 1, duration::zero());
  }

  registry.emplace<duration>(entity_id, make_duration(1.f / rate));

  if (looped)
  {
    registry.emplace<SpriteSequenceLooped>(entity_id);
  }
  else
  {
    registry.emplace<SpriteSequenceOneShot>(entity_id);
  }
}

void detach_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id)
{
  TYL_ASSERT_TRUE(registry.has<SpriteSequence>(entity_id));
  registry.remove<SpriteSequence, duration>(entity_id);
  registry.remove_if_exists<SpriteSequenceOneShot>(entity_id);
  registry.remove_if_exists<SpriteSequenceLooped>(entity_id);
}


void retrigger_sprite_sequence(const ecs::Ref<SpriteSequence, SpriteTileID> sprite)
{
  auto [sequence, tile] = sprite.get<SpriteSequence, SpriteTileID>();
  tile.id = sequence.start_id;
  sequence.period_since_last_update = duration::zero();
}

}  // namespace tyl::graphics
