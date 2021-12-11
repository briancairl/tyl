/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite_sequence.cpp
 */

// C++ Standard Library
#include <optional>

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

void attach_sprite_batch_renderer_shader(ecs::registry& registry, const ecs::entity e)
{
  // clang-format off
  registry.emplace<Shader>(
    e,
    ShaderSource::vertex(
      R"VertexShader(

      // Vertex-buffer layout
      layout (location = 0) in vec2 layout_UnitPosition;
      layout (location = 1) in vec2 layout_UnitTexCoord;
      layout (location = 2) in vec4 layout_PositionRect;  // { corner, extents }
      layout (location = 3) in vec4 layout_TexCoordRect;  // { corner, extents }

      // Texture UV coordinate output
      out vec2 vshader_TexCoord;

      // View-projection matrix uniform
      uniform mat3 u_ViewProjection;

      void main()
      {
        gl_Position =
          vec4(
            u_ViewProjection *
            vec3(layout_PositionRect[0] + layout_UnitPosition[0] * layout_PositionRect[2],
                 layout_PositionRect[1] + layout_UnitPosition[1] * layout_PositionRect[3],
                 1),
            1
          );

        vshader_TexCoord =
          vec2(
            layout_TexCoordRect[0] + layout_UnitTexCoord[0] * layout_TexCoordRect[2],
            layout_TexCoordRect[1] + layout_UnitTexCoord[1] * layout_TexCoordRect[3]
          );
      }

      )VertexShader"
    ),
    ShaderSource::fragment(
      R"FragmentShader(

      // Fragment color
      out vec4 FragColor;

      // Texture UV coordinate from vertex shader
      in vec2 vshader_TexCoord;

      // Texture sampling unit uniform
      uniform sampler2D u_TextureID;

      void main()
      {
        FragColor = texture(u_TextureID, vshader_TexCoord);
      }

      )FragmentShader"
    )
  );
}

static constexpr std::size_t SPRITE_QUAD_POSITION_INDEX = 0;
static constexpr std::size_t SPRITE_QUAD_TEXCOORD_INDEX = 1;
static constexpr std::size_t SPRITE_OFFSET_POSITION_INDEX = 2;
static constexpr std::size_t SPRITE_OFFSET_TEXCOORD_INDEX = 3;

void attach_sprite_batch_renderer_vertex_buffer(ecs::registry& registry, const ecs::entity e, const std::size_t sprite_count)
{
  // clang-format off
  registry.emplace<VertexBuffer>(
    e,
    [sprite_count] {
      VertexBuffer vb{6UL,
                      {
                        VertexBuffer::Attribute{device::typecode<float>(), 2, 4, 0},             // position unit quad
                        VertexBuffer::Attribute{device::typecode<float>(), 2, 4, 0},             // texcoord unit quad
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // position [corner, extents]
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // texcoord [corner, extents]
                      },
                      VertexBuffer::BufferMode::DYNAMIC};
      {
        const unsigned indices[6UL] = {0, 1, 2, 2, 3, 0};
        vb.set_index_data(indices);
      }

      {
        const Vec2f quad[4UL] = {
          Vec2f{0.f, 0.f},
          Vec2f{1.f, 0.f},
          Vec2f{1.f, 1.f},
          Vec2f{0.f, 1.f},
        };
        vb.set_vertex_data(SPRITE_QUAD_POSITION_INDEX, reinterpret_cast<const float*>(quad));
      }

      {
        const Vec2f quad[4UL] = {
          Vec2f{0.f, 1.f},
          Vec2f{1.f, 1.f},
          Vec2f{1.f, 0.f},
          Vec2f{0.f, 0.f},
        };
        vb.set_vertex_data(SPRITE_QUAD_TEXCOORD_INDEX, reinterpret_cast<const float*>(quad));
      }
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
        if (auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(SPRITE_OFFSET_POSITION_INDEX); vb_buffer_ptr)
        {
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
        vertex_buffer.draw_instanced(sprite_count);
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


void retrigger_sprite_sequence(ecs::registry& registry, const ecs::entity entity_id)
{
  TYL_ASSERT_TRUE((registry.has<SpriteSequence, SpriteTileID>(entity_id)));
  auto [sequence, tile] = registry.get<SpriteSequence, SpriteTileID>(entity_id);
  tile.id = sequence.start_id;
}

}  // namespace tyl::graphics
