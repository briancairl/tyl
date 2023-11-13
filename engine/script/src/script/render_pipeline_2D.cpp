/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_pipeline_2D.cpp
 */

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/assert.hpp>
#include <tyl/engine/camera.hpp>
#include <tyl/engine/drawing.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/render_pipeline_2D.hpp>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>


namespace tyl::engine
{

using namespace tyl::graphics::device;
using namespace tyl::serialization;

namespace
{

static constexpr const char* kPrimitivesVertexShaderSource =
  R"VertexShader(

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

out vec4 vFragColor;
uniform mat3 uCameraTransform;

void main()
{
  gl_Position = vec4(uCameraTransform * vPos, 1);
  vFragColor = vColor;
}

)VertexShader";

static constexpr const char* kPrimitivesFragmentShaderSource =
  R"FragmentShader(

layout(location = 0) out vec4 FragColor;

in vec4 vFragColor;

void main()
{
  FragColor = vFragColor;
}

)FragmentShader";

struct PrimitivesVertexBuffer
{
  using Position = VertexAttribute<float, 3>;
  using Color = VertexAttribute<float, 4>;

  VertexAttributeBuffer<float> position;
  VertexAttributeBuffer<float> color;

  VertexBuffer vb;

  std::size_t max_vertex_count;

  static PrimitivesVertexBuffer create(const std::size_t max_vertex_count)
  {
    auto [vb, position, color] =
      VertexBuffer::create(VertexBuffer::BufferMode::kDynamic, Position{max_vertex_count}, Color{max_vertex_count});

    return {
      .position = std::move(position),
      .color = std::move(color),
      .vb = std::move(vb),
      .max_vertex_count = max_vertex_count};
  }
};

template <typename DrawTypeT> struct PrimitiveDrawMode;

template <>
struct PrimitiveDrawMode<LineList2D> : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::kLines>
{};

template <>
struct PrimitiveDrawMode<LineStrip2D>
    : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::kLineStrip>
{};


template <>
struct PrimitiveDrawMode<Points2D> : std::integral_constant<VertexBuffer::DrawMode, VertexBuffer::DrawMode::kPoints>
{};

template <typename PrimitiveT, typename SetVertexT>
void DrawPrimitivesSingleColor(PrimitivesVertexBuffer& dvb, const Registry& registry, SetVertexT set_vertex)
{
  static constexpr bool IsLineStrip =
    std::is_same<PrimitiveT, LineStrip2D>() or std::is_same<PrimitiveT, LineStrip3D>();

  auto view = registry.template view<PrimitiveT, Color>();
  std::size_t vertex_pos = 0;
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    for (const auto e : view)
    {
      const auto& vertices = view.template get<PrimitiveT>(e).values;

      // Skip empty vertex lists
      if (vertices.empty())
      {
        continue;
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_pos != 0 and vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertices.front());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_pos + vertices.size() > dvb.max_vertex_count)
      {
        break;
      }

      // Add vertex data
      const auto& vertex_color = view.template get<Color>(e);
      for (const auto& v : vertices)
      {
        set_vertex(position_ptr[vertex_pos], v);
        color_ptr[vertex_pos] = vertex_color.rgba;
        ++vertex_pos;
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertices.back());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }
    }
  }

  if (vertex_pos > 0)
  {
    dvb.vb.draw(vertex_pos, PrimitiveDrawMode<PrimitiveT>());
  }
}

template <typename PrimitiveT, typename SetVertexT>
void DrawPrimitivesMultiColor(PrimitivesVertexBuffer& dvb, const Registry& registry, SetVertexT set_vertex)
{
  static constexpr bool IsLineStrip =
    std::is_same<PrimitiveT, LineStrip2D>() or std::is_same<PrimitiveT, LineStrip3D>();

  auto view = registry.template view<PrimitiveT, ColorList>();
  std::size_t vertex_pos = 0;
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    for (const auto e : view)
    {
      const auto& vertices = view.template get<PrimitiveT>(e).values;
      const auto& colors = view.template get<ColorList>(e).values;

      TYL_ASSERT_EQ(vertices.size(), colors.size());

      // Skip empty vertex lists
      if (vertices.empty())
      {
        continue;
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_pos != 0 and vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertices.front());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_pos + vertices.size() > dvb.max_vertex_count)
      {
        break;
      }

      // Add vertex data
      for (std::size_t i = 0; i < vertices.size(); ++i)
      {
        set_vertex(position_ptr[vertex_pos], vertices[i]);
        color_ptr[vertex_pos] = colors[i].rgba;
        ++vertex_pos;
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_pos < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_pos], vertices.back());
          color_ptr[vertex_pos] = Vec4f::Zero();
          ++vertex_pos;
        }
      }
    }
  }

  if (vertex_pos > 0)
  {
    dvb.vb.draw(vertex_pos, PrimitiveDrawMode<PrimitiveT>());
  }
}

}  // namespace

class RenderPipeline2D::Impl
{
public:
  Impl(Shader&& shader, PrimitivesVertexBuffer&& vb) :
      primitives_shader_{std::move(shader)}, primitives_vb_{std::move(vb)}
  {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    primitives_shader_.bind();

    const auto SetVertexFrom2D = [](auto& dst, const auto& src) {
      dst.template head<2>() = src;
      dst[2] = 1.f;
    };


    scene.graphics.view<TopDownCamera2D>().each([&](const TopDownCamera2D& camera) {
      const auto camera_matrix = ToInverseCameraMatrix(camera);
      primitives_shader_.setMat3("uCameraTransform", camera_matrix.data());
      DrawPrimitivesSingleColor<LineList2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
      DrawPrimitivesSingleColor<LineStrip2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
      DrawPrimitivesSingleColor<Points2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
      DrawPrimitivesMultiColor<LineList2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
      DrawPrimitivesMultiColor<LineStrip2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
      DrawPrimitivesMultiColor<Points2D>(primitives_vb_, scene.graphics, SetVertexFrom2D);
    });
  }

private:
  Shader primitives_shader_;
  PrimitivesVertexBuffer primitives_vb_;
};

RenderPipeline2D::~RenderPipeline2D() = default;

tyl::expected<RenderPipeline2D, ScriptCreationError>
RenderPipeline2D::CreateImpl(const RenderPipeline2DOptions& options)
{
  if (auto vertex_shader = ShaderSource::vertex(kPrimitivesVertexShaderSource); !vertex_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto fragment_shader = ShaderSource::fragment(kPrimitivesFragmentShaderSource); !fragment_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto primitives_shader = Shader::create(*vertex_shader, *fragment_shader); !primitives_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else
  {
    return RenderPipeline2D{
      options,
      std::make_unique<Impl>(
        std::move(primitives_shader).value(), PrimitivesVertexBuffer::create(options.max_vertex_count))};
  }
}

RenderPipeline2D::RenderPipeline2D(const RenderPipeline2DOptions& options, std::unique_ptr<Impl>&& impl) :
    impl_{std::move(impl)}
{}

template <> void RenderPipeline2D::SaveImpl(ScriptOArchive<file_handle_ostream>& oar) const {}  // impl_->Save(oar); }

template <> void RenderPipeline2D::LoadImpl(ScriptIArchive<file_handle_istream>& iar) {}  // impl_->Load(iar); }

ScriptStatus RenderPipeline2D::UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
{
  impl_->Update(scene, shared, resources);
  return ScriptStatus::kOk;
}

}  // namespace tyl::engine