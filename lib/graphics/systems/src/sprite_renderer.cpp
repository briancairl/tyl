/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_renderer.cpp
 */

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/graphics/sprite/animation.hpp>
#include <tyl/graphics/sprite/spritesheet.hpp>
#include <tyl/graphics/systems/render_target.hpp>
#include <tyl/graphics/systems/renderable.hpp>
#include <tyl/graphics/systems/sprite_renderer.hpp>
#include <tyl/math/size.hpp>
#include <tyl/math/vec.hpp>

namespace tyl::graphics::systems
{
namespace  // anonymous
{

struct VertexBuffer
{
  device::VertexElementBuffer vb;
  device::VertexElementBufferLayout element_buffer;
  device::VertexAttributeBuffer<float> position_buffer;
  device::VertexAttributeBuffer<float> texcoord_buffer;
};

void attach_sprite_renderer_shader(ecs::registry& registry, const ecs::entity e)
{
  // clang-format off
  registry.emplace<device::Shader>(
    e,
    device::ShaderSource::vertex(
      R"VertexShader(
        // Vertex-buffer layout
        layout (location = 0) in vec2 lo_Position;
        layout (location = 1) in vec2 lo_TexCoord;

        // Texture UV coordinate output
        out vec2 vs_TexCoord;

        // View-projection matrix uniform
        uniform mat3 uniform_ViewProjection;

        void main()
        {
          gl_Position = vec4(uniform_ViewProjection * vec3(lo_Position, 1), 1);
          vs_TexCoord = lo_TexCoord;
        }

      )VertexShader"
    ),
    device::ShaderSource::fragment(
      R"FragmentShader(
        // Fragment color
        out vec4 fs_Color;

        // Texture UV coordinate from vertex shader
        in vec2 vs_TexCoord;

        // Texture sampling unit uniform
        uniform sampler2D uniform_TextureID;

        void main()
        {
          fs_Color = texture(uniform_TextureID, vs_TexCoord);
        }
      )FragmentShader"
    )
  );
}

} // namespace anonymous

void attach_sprite_renderer(ecs::registry& reg, const ecs::entity e, const SpriteRendererOptions& options)
{
  TYL_ASSERT_GT(options.capacity, 0UL);

  // Add options/state
  reg.emplace<SpriteRendererOptions>(e, options);
  reg.emplace<SpriteRendererState>(e, 0UL);

  // Allocate VBOs for sprite rendering
  {
    auto [vb, element_buffer, position_buffer, texcoord_buffer] = device::VertexElementBuffer::create(
      device::VertexBuffer::BufferMode::Dynamic,
      (6UL * options.capacity),
      device::VertexAttribute<float, 2>{4UL * options.capacity},
      device::VertexAttribute<float, 2>{4UL * options.capacity}
    );

    // Fill element indices for all potential quads
    {
      auto mapped = vb.get_mapped_element_buffer_write();
      unsigned int* const data = mapped(element_buffer);

      //
      // 0 --- 3
      // | \   |
      // |  \  |
      // |   \ |
      // 1 --- 2      
      // 

      unsigned int* p = data; 
      unsigned int index_offset = 0;
      for (std::size_t q = 0; q < options.capacity; ++q, p += 6UL, index_offset += 4UL)
      {
        p[0] = 0 + index_offset;
        p[1] = 1 + index_offset;
        p[2] = 2 + index_offset;
        p[3] = 0 + index_offset;
        p[4] = 3 + index_offset;
        p[5] = 2 + index_offset;
      }
    }

    reg.emplace<VertexBuffer>(e, std::move(vb), std::move(element_buffer), std::move(position_buffer), std::move(texcoord_buffer));
  }

  // Create and attach a shader program
  attach_sprite_renderer_shader(reg, e);
}

ecs::entity create_sprite_renderer(ecs::registry& reg, const AtlasTextureReference& atlas_texture, const SpriteRendererOptions& options)
{
  TYL_ASSERT_GT(options.capacity, 0UL);

  const auto e = reg.create();
  reg.emplace<AtlasTextureReference>(e, atlas_texture);
  attach_sprite_renderer(reg, e, options);

  return e;
}

void update_sprite_renderers(ecs::registry& reg, const RenderTarget2D& target)
{
  reg.view<SpriteRendererState, SpriteRendererOptions, VertexBuffer, AtlasTextureReference, device::Shader>().each(
    [&target, &reg](auto& render_state, const auto& render_options, const auto& render_vertex_buffer, const auto& render_atlas_texture, const auto& render_shader)
    {
      // Bind texture to unit
      render_atlas_texture->bind(render_options.atlas_texture_unit);

      // Bind shader program
      render_shader.bind();

      // Set shader uniformrs
      render_shader.setInt("uniform_TextureID", render_options.atlas_texture_unit);
      render_shader.setMat3("uniform_ViewProjection", target.view_projection.data());

      // Get mapped vertex buffer pointers
      {
        auto mapped = render_vertex_buffer.vb.get_mapped_vertex_buffer_write();
        auto* const position_begin = reinterpret_cast<tyl::Vec2f*>(mapped(render_vertex_buffer.position_buffer));
        auto* const texcoord_begin = reinterpret_cast<tyl::Vec2f*>(mapped(render_vertex_buffer.texcoord_buffer));

        render_state.size = 0;

        // Render animated sprites
        auto view = reg.view<tags::rendering_enabled, tyl::Vec2f, tyl::Size2f, sprite::AnimationFrames, sprite::AnimationState, sprite::AnimationProperties>();
        for (const auto e : view)
        {
          if (render_state.size == render_options.capacity)
          {
            break;
          }

          const std::size_t offset = render_state.size * 4UL;

          const auto [pos, extents, ani_frames, ani_state, ani_props] =
            view.get<tyl::Vec2f, tyl::Size2f, sprite::AnimationFrames, sprite::AnimationState, sprite::AnimationProperties>(e);

          // Set vertex positions
          {
            auto* const v_pos = position_begin + offset;
    
            v_pos[0] = pos;

            v_pos[1].x() = pos.x();
            v_pos[1].y() = pos.y() + extents.y();

            v_pos[2] = pos + extents;

            v_pos[3].x() = pos.x() + extents.x();
            v_pos[3].y() = pos.y();
          }

          // Set vertex texcoords
          {
            const auto& uv_bounds = sprite::get_frame(ani_frames, ani_state);

            auto* const v_tex = texcoord_begin + offset;

            v_tex[0].x() = uv_bounds.min().x();
            v_tex[0].y() = uv_bounds.max().y();

            v_tex[1] = uv_bounds.min();

            v_tex[2].x() = uv_bounds.max().x();
            v_tex[2].y() = uv_bounds.min().y();

            v_tex[3] = uv_bounds.max();
          }

          ++render_state.size;
        }
      }

      render_vertex_buffer.vb.draw(6 * render_state.size);
    }
  );
}

}  // namespace tyl::graphics::systems