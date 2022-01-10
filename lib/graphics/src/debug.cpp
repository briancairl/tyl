/**
 * @copyright 2022-present Brian Cairl
 *
 * @file debug.cpp
 */

// Tyl
#include <tyl/assert.hpp>
#include <tyl/debug.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/debug.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/target.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/vec.hpp>

namespace tyl::graphics
{
namespace  // anonymous
{

void attach_bounding_box_batch_renderer_shader(ecs::registry& registry, const ecs::entity e)
{
  // clang-format off
  registry.emplace<Shader>(
    e,
    ShaderSource::vertex(
      R"VertexShader(

      // Vertex-buffer layout
      layout (location = 0) in vec4 layout_VertexPosition;
      layout (location = 1) in vec4 layout_Color;

      // Fragment color
      out vec4 VertColor;

      void main()
      {
        gl_Position = layout_VertexPosition;
        VertColor = layout_Color;
      }

      )VertexShader"
    ),
    ShaderSource::fragment(
      R"FragmentShader(

      // Fragment color
      out vec4 FragColor;

      // Color of the bounding box
      in vec4 GeomColor;

      void main()
      {
        FragColor = GeomColor;
      }

      )FragmentShader"
    ),
    ShaderSource::geometry(
      R"GeometryShader(
      layout (points) in;
      layout (line_strip, max_vertices = 5) out;

      // Vertex input color
      in vec4 VertColor[];

      // Vertex output color
      out vec4 GeomColor;

      // View-projection matrix uniform
      uniform mat3 u_ViewProjection;

      void main()
      {
        float x_lower = gl_in[0].gl_Position[0];
        float y_lower = gl_in[0].gl_Position[1];
        float x_upper = gl_in[0].gl_Position[2];
        float y_upper = gl_in[0].gl_Position[3];

        vec4 start = vec4(u_ViewProjection * vec3(x_lower, y_lower, 1), 1);
        gl_Position = start;
        GeomColor = VertColor[0];
        EmitVertex();
        gl_Position = vec4(u_ViewProjection * vec3(x_lower, y_upper, 1), 1);
        GeomColor = VertColor[0];
        EmitVertex();
        gl_Position = vec4(u_ViewProjection * vec3(x_upper, y_upper, 1), 1);
        GeomColor = VertColor[0];
        EmitVertex();
        gl_Position = vec4(u_ViewProjection * vec3(x_upper, y_lower, 1), 1);
        GeomColor = VertColor[0];
        EmitVertex();
        gl_Position = start;
        GeomColor = VertColor[0];
        EmitVertex();
        EndPrimitive();
      }

      )GeometryShader"
    )
  );
}

static constexpr std::size_t BB_OFFSET_POSITION_INDEX = 0;
static constexpr std::size_t BB_OFFSET_COLOR_INDEX = 1;

void attach_bounding_box_batch_renderer_vertex_buffer(ecs::registry& registry, const ecs::entity e, const std::size_t bounding_box_count)
{
  // clang-format off
  registry.emplace<VertexBuffer>(
    e,
    [bounding_box_count] {
      VertexBuffer vb{{
                        VertexBuffer::Attribute{device::typecode<float>(), 4, bounding_box_count, 0}, // [lower corner, upper corner],
                        VertexBuffer::Attribute{device::typecode<float>(), 4, bounding_box_count, 0}  // [r, g, b, a]
                      },
                      VertexBuffer::BufferMode::DYNAMIC};
      return vb;
    }());
  // clang-format on
}

}  // namespace anonymous

ecs::entity create_bounding_box_batch_renderer(ecs::registry& registry, const std::size_t max_bounding_box_count)
{
  const ecs::entity entity_id = registry.create();
  attach_bounding_box_batch_renderer(registry, entity_id, max_bounding_box_count);
  return entity_id;
}

void attach_bounding_box_batch_renderer(
  ecs::registry& registry,
  const ecs::entity entity_id,
  const std::size_t max_bounding_box_count)
{
  attach_bounding_box_batch_renderer_vertex_buffer(registry, entity_id, max_bounding_box_count);
  attach_bounding_box_batch_renderer_shader(registry, entity_id);
  registry.emplace<BoundingBoxBatchRenderProperties>(entity_id, max_bounding_box_count);
}

struct IsVisible
{};

void draw_bounding_boxes(ecs::registry& registry, Target& render_target, const duration dt)
{
  registry.view<ViewProjection, ViewportRect>().each([&](
                                                       const ViewProjection& view_projection,
                                                       const ViewportRect& viewport_rect) {
    registry.view<BoundingBoxBatchRenderProperties, VertexBuffer, Shader>().each(
      [&](
        const BoundingBoxBatchRenderProperties& render_props, const VertexBuffer& vertex_buffer, const Shader& shader) {
        // Set shader program if its not already active
        render_target.bind(shader);

        // Set view projection matrix
        shader.setMat3("u_ViewProjection", reinterpret_cast<const float*>(std::addressof(view_projection)));

        std::size_t bounding_box_count = 0;

        // Figure out which boxes to render
        {
          auto bounding_box_view = registry.view<Rect2D, BoundingBoxColor>();
          for (const auto bounding_box_id : bounding_box_view)
          {
            if (const auto& rect = bounding_box_view.get<Rect2D>(bounding_box_id); viewport_rect.intersects(rect))
            {
              registry.emplace<IsVisible>(bounding_box_id);
              ++bounding_box_count;
            }
          }
        }

        auto bounding_box_view = registry.view<Rect2D, BoundingBoxColor, IsVisible>();

        // Buffer corner data
        if (auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(BB_OFFSET_POSITION_INDEX); vb_buffer_ptr)
        {
          auto corner_data_ptr = vb_buffer_ptr.as<Vec4f>();
          for (const auto bounding_box_id : bounding_box_view)
          {
            const auto& rect = bounding_box_view.get<Rect2D>(bounding_box_id);
            corner_data_ptr->head<2>() = rect.min();
            corner_data_ptr->tail<2>() = rect.max();
            ++corner_data_ptr;
          }
        }

        // Buffer color data
        if (auto vb_buffer_ptr = vertex_buffer.get_vertex_ptr(BB_OFFSET_COLOR_INDEX); vb_buffer_ptr)
        {
          auto color_data_ptr = vb_buffer_ptr.as<Vec4f>();
          for (const auto bounding_box_id : bounding_box_view)
          {
            (*color_data_ptr) = bounding_box_view.get<BoundingBoxColor>(bounding_box_id);
            ++color_data_ptr;
          }
        }

        // Draw all the sprites that we buffered
        vertex_buffer.draw(bounding_box_count, VertexBuffer::DrawMode::POINTS);
      });

    // Remove visibility flag
    {
      auto bounding_box_view = registry.view<Rect2D, BoundingBoxColor, IsVisible>();
      for (const auto bounding_box_id : bounding_box_view)
      {
        registry.remove<IsVisible>(bounding_box_id);
      }
    }
  });
}

}  // namespace tyl::graphics
