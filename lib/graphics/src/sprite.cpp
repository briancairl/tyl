/**
 * @copyright 2021-present Brian Cairl
 *
 * @file sprite.cpp
 */

// C++ Standard Library
#include <optional>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/components.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/device/typecode.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/sprite.hpp>
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
      layout (location = 2) in vec4 layout_PositionSpec;  // { offset[0, 1], size[2, 3] }
      layout (location = 3) in vec4 layout_TexCoordSpec;  // { offset[0, 1], size[2, 3] }

      // Texture UV coordinate output
      out vec2 vshader_TexCoord;

      // View-projection matrix uniform
      uniform mat3 u_ViewProjection;

      void main()
      {
        vec2 pos = vec2(layout_PositionSpec[0] + layout_UnitPosition[0] * layout_PositionSpec[2],
                        layout_PositionSpec[1] + layout_UnitPosition[1] * layout_PositionSpec[3]);

        vec2 puv = vec2(layout_TexCoordSpec[0] + layout_UnitTexCoord[0] * layout_TexCoordSpec[2],
                        layout_TexCoordSpec[1] + layout_UnitTexCoord[1] * layout_TexCoordSpec[3]);

        gl_Position =  vec4(u_ViewProjection * vec3(pos, 1), 1);
        vshader_TexCoord = puv;
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
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // position [offset, quad size]
                        VertexBuffer::Attribute{device::typecode<float>(), 4, sprite_count, 1},  // texcoord [offset, quad size]
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

void render_sprites(ecs::registry& registry, const Vec2i& viewport_size)
{
  using W_Texture = ecs::Ref<Texture>;
  using W_TileUVLookup = ecs::Ref<TileUVLookup>;

  std::optional<device::shader_id_t> active_shader_id{std::nullopt};
  std::optional<device::texture_id_t> active_texture_id{std::nullopt};

  registry.view<CameraTopDown>().each([&](const CameraTopDown& camera) {
    const auto view_projection = camera.get_view_projection_matrix(viewport_size);

    registry.view<SpriteBatchRenderProperties, VertexBuffer, Shader>().each(
      [&](const auto& render_props, const auto& vertex_buffer, const auto& shader) {
        // Set shader program if its not already active
        if (!active_shader_id or active_shader_id != shader.get_id())
        {
          shader.bind();
          shader.setMat3("u_ViewProjection", reinterpret_cast<const float*>(std::addressof(view_projection)));
          active_shader_id = shader.get_id();
        }

        // Buffer sprite data (position, uv)
        std::size_t sprite_count = 0;
        if (auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(SPRITE_OFFSET_POSITION_INDEX); vb_buffer_ptr)
        {
          auto position_data = vb_buffer_ptr.template as<Vec4f>();
          auto texcoord_data = position_data + render_props.max_sprite_count;
          auto sprite_view = registry.template view<Position2D, RectSize2D, SpriteTileID, W_Texture, W_TileUVLookup>();
          for (const auto sprite_id : sprite_view)
          {
            // Stop buffering sprites if we hit the max sprite count
            if (sprite_count > render_props.max_sprite_count)
            {
              break;
            }
            // Set active texture unit if its not already active
            else if (const Texture& texture = sprite_view.template get<W_Texture>(sprite_id);
                     !active_texture_id or active_texture_id != texture.get_id())
            {
              texture.bind(0);
              shader.setInt("u_TextureID", 0);
              active_texture_id = texture.get_id();
            }

            // Set sprite position info
            {
              position_data->template head<2>() = sprite_view.template get<Position2D>(sprite_id);
              position_data->template tail<2>() = sprite_view.template get<RectSize2D>(sprite_id);
            }

            // Set sprite tile info
            {
              const TileUVLookup& uv_lookup = sprite_view.template get<W_TileUVLookup>(sprite_id);
              const SpriteTileID& tile = sprite_view.template get<SpriteTileID>(sprite_id);
              TYL_ASSERT_LT(tile.id, uv_lookup.tile_count());
              texcoord_data->template head<2>() = uv_lookup[tile.id];
              texcoord_data->template tail<2>() = uv_lookup.tile_size_uv();
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
  });
}

ecs::entity create_sprite(
  ecs::registry& registry,
  ecs::Ref<Texture> atlas_texture,
  ecs::Ref<TileUVLookup> uv_lookup,
  const Position2D& sprite_position,
  const RectSize2D& sprite_size)
{
  const ecs::entity entity_id = registry.create();
  attach_sprite(registry, entity_id, std::move(atlas_texture), std::move(uv_lookup), sprite_position, sprite_size);
  return entity_id;
}

void attach_sprite(
  ecs::registry& registry,
  const ecs::entity entity_id,
  ecs::Ref<Texture> atlas_texture,
  ecs::Ref<TileUVLookup> uv_lookup,
  const Position2D& sprite_position,
  const RectSize2D& sprite_size)
{
  registry.emplace<ecs::Ref<Texture>>(entity_id, std::move(atlas_texture));
  registry.emplace<ecs::Ref<TileUVLookup>>(entity_id, std::move(uv_lookup));
  registry.emplace<Position2D>(entity_id, sprite_position);
  registry.emplace<RectSize2D>(entity_id, sprite_size);
  registry.emplace<SpriteTileID>(entity_id, 0UL);
}

}  // namespace tyl::graphics
