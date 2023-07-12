/**
 * @copyright 2023-present Brian Cairl
 *
 * @file primitives_renderer.cpp
 */

// C++ Standard Library
#include <optional>
#include <type_traits>

// Tyl
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/graphics/engine/primitives_renderer.hpp>
#include <tyl/graphics/engine/types.hpp>

using namespace tyl::graphics::device;

namespace tyl::engine::graphics
{

namespace
{

static constexpr const char* VertexShaderSource =
  R"VertexShader(

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

out vec4 vFragColor;

void main()
{
  gl_Position = vec4(vPos, 1);
  vFragColor = vColor;
}

)VertexShader";

static constexpr const char* FragmentShaderSource =
  R"FragmentShader(

layout(location = 0) out vec4 FragColor;

in vec4 vFragColor;

void main()
{
  FragColor = vFragColor;
}

)FragmentShader";

struct DrawingVertexBuffer
{
  using Position = VertexAttribute<float, 3>;
  using Color = VertexAttribute<float, 4>;

  VertexAttributeBuffer<float> position;
  VertexAttributeBuffer<float> color;

  VertexBuffer vb;

  std::size_t max_vertex_count;

  static DrawingVertexBuffer create(const std::size_t max_vertex_count)
  {
    auto [vb, position, color] =
      VertexBuffer::create(VertexBuffer::BufferMode::Dynamic, Position{max_vertex_count}, Color{max_vertex_count});
    return DrawingVertexBuffer{
      .position = std::move(position),
      .color = std::move(color),
      .vb = std::move(vb),
      .max_vertex_count = max_vertex_count};
  }
};

template <typename DrawTypeT> struct VertexBufferDrawMode;

template <>
struct VertexBufferDrawMode<DrawType::LineList>
    : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::Lines>
{};

template <>
struct VertexBufferDrawMode<DrawType::LineStrip>
    : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::LineStrip>
{};

template <>
struct VertexBufferDrawMode<DrawType::Points>
    : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::Points>
{};

template <typename VertexListT, typename DrawT, typename SetVertexT>
void draw_primitive_with_single_color(DrawingVertexBuffer& dvb, const entt::registry& registry, SetVertexT set_vertex)
{
  auto view = registry.template view<DrawT, VertexListT, VertexColor>();
  std::size_t vertex_pos = 0;
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    for (const auto e : view)
    {
      const auto& vertex_list = view.template get<VertexListT>(e);

      // Skip empty vertex lists
      if (vertex_list.empty())
      {
        continue;
      }

      // Add dummy line for batched vertex strips
      if constexpr (std::is_same<DrawT, DrawType::LineStrip>())
      {
        if (vertex_pos != 0 and vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertex_list.front());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_pos + vertex_list.size() > dvb.max_vertex_count)
      {
        break;
      }

      // Add vertex data
      const auto& vertex_color = view.template get<VertexColor>(e);
      for (const auto& v : vertex_list)
      {
        set_vertex(position_ptr[vertex_pos], v);
        color_ptr[vertex_pos] = vertex_color;
        ++vertex_pos;
      }

      // Add dummy line for batched vertex strips
      if constexpr (std::is_same<DrawT, DrawType::LineStrip>())
      {
        if (vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertex_list.back());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }
    }
  }
  dvb.vb.draw(vertex_pos, VertexBufferDrawMode<DrawT>::value);
}

}  // namespace

class PrimitivesRenderer::Impl
{
public:
  Impl(Shader&& shader, DrawingVertexBuffer&& vb) : shader_{std::move(shader)}, vb_{std::move(vb)} {}

  void run(const entt::registry& registry)
  {
    shader_.bind();
    const auto set_vertex_from_2d = [](auto& dst, const auto& src) {
      dst.template head<2>() = src;
      dst[2] = 0.f;
    };
    draw_primitive_with_single_color<VertexList2D, DrawType::LineList>(vb_, registry, set_vertex_from_2d);
    draw_primitive_with_single_color<VertexList2D, DrawType::LineStrip>(vb_, registry, set_vertex_from_2d);
    draw_primitive_with_single_color<VertexList2D, DrawType::Points>(vb_, registry, set_vertex_from_2d);
  }

private:
  Shader shader_;
  DrawingVertexBuffer vb_;
};

PrimitivesRenderer::~PrimitivesRenderer() = default;

tyl::expected<PrimitivesRenderer, PrimitivesRenderer::ErrorCode> PrimitivesRenderer::create(const Settings& settings)
{
  if (auto vertex_shader = ShaderSource::vertex(VertexShaderSource); !vertex_shader)
  {
    return tyl::unexpected{ErrorCode::SHADER_CREATION_FAILURE_VERTEX};
  }
  else if (auto fragment_shader = ShaderSource::fragment(FragmentShaderSource); !fragment_shader)
  {
    return tyl::unexpected{ErrorCode::SHADER_CREATION_FAILURE_FRAGMENT};
  }
  else if (auto shader = Shader::create(*vertex_shader, *fragment_shader); !shader)
  {
    return tyl::unexpected{ErrorCode::SHADER_LINKAGE_FAILURE};
  }
  else
  {
    return PrimitivesRenderer{
      std::make_unique<Impl>(std::move(shader).value(), DrawingVertexBuffer::create(settings.max_vertex_count))};
  }
}

PrimitivesRenderer::PrimitivesRenderer(std::unique_ptr<Impl>&& impl) : impl_{std::move(impl)} {}

void PrimitivesRenderer::draw(const entt::registry& registry) { impl_->run(registry); }

}  // namespace tyl::engine::graphics
