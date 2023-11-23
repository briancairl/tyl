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
#include <tyl/engine/math.hpp>
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

template <typename PrimitiveT, typename ColorT, typename SetVertexT>
std::size_t AddPrimitives(
  PrimitivesVertexBuffer& dvb,
  const Registry& registry,
  SetVertexT set_vertex,
  std::size_t vertex_count = 0)
{
  static constexpr bool IsLineStrip =
    std::is_same<PrimitiveT, LineStrip2D>() or std::is_same<PrimitiveT, LineStrip3D>();
  static constexpr bool IsSingleColor = std::is_same_v<ColorT, Color>;

  auto view = registry.template view<PrimitiveT, ColorT>();
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    for (const auto e : view)
    {
      const auto& vertices = view.template get<PrimitiveT>(e).values;
      const auto& vertex_color = view.template get<ColorT>(e);

      // Skip empty vertex lists
      if (vertices.empty())
      {
        continue;
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_count != 0 and vertex_count < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_count], vertices.front());
          color_ptr[vertex_count] = Vec4f::Zero();
          ++vertex_count;
        }
      }

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_count + vertices.size() > dvb.max_vertex_count)
      {
        break;
      }

      // Add vertex data
      for (std::size_t i = 0; i < vertices.size(); ++i, ++vertex_count)
      {
        set_vertex(position_ptr[vertex_count], vertices[i]);
        if constexpr (IsSingleColor)
        {
          color_ptr[vertex_count] = vertex_color.rgba;
        }
        else
        {
          color_ptr[vertex_count] = vertex_color.values[i].rgba;
        }
      }

      // Add dummy line for batched vertex strips
      if constexpr (IsLineStrip)
      {
        if (vertex_count < dvb.max_vertex_count)
        {
          set_vertex(position_ptr[vertex_count], vertices.back());
          color_ptr[vertex_count] = Vec4f::Zero();
          ++vertex_count;
        }
      }
    }
  }
  return vertex_count;
}

template <typename PrimitiveT, typename SetVertexT>
std::size_t SubmitPrimitives(
  PrimitivesVertexBuffer& dvb,
  const Registry& registry,
  SetVertexT&& set_vertex,
  std::size_t vertex_count)
{
  vertex_count =
    AddPrimitives<PrimitiveT, Color, SetVertexT>(dvb, registry, std::forward<SetVertexT>(set_vertex), vertex_count);
  vertex_count =
    AddPrimitives<PrimitiveT, ColorList, SetVertexT>(dvb, registry, std::forward<SetVertexT>(set_vertex), vertex_count);
  return vertex_count;
}

std::size_t SubmitRect2fAsLineStrips(PrimitivesVertexBuffer& dvb, const Registry& registry, std::size_t vertex_count)
{
  auto view = registry.view<Rect2f, Color>();
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    for (const auto e : view)
    {
      static constexpr std::size_t kPoints = 5;

      const auto& rect = view.template get<Rect2f>(e);
      const auto& color = view.template get<Color>(e);

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_count + kPoints > dvb.max_vertex_count)
      {
        break;
      }

      const Vec3f corners[kPoints] = {
        {rect.min().x(), rect.min().y(), 0.f},
        {rect.min().x(), rect.max().y(), 0.f},
        {rect.max().x(), rect.max().y(), 0.f},
        {rect.max().x(), rect.min().y(), 0.f},
        {rect.min().x(), rect.min().y(), 0.f},
      };

      for (const auto& pos : corners)
      {
        position_ptr[vertex_count] = pos;
        color_ptr[vertex_count] = color.rgba;
        ++vertex_count;
      }
    }
  }
  return vertex_count;
}

template <typename PrimitiveT> void DrawPrimitives(PrimitivesVertexBuffer& dvb, std::size_t vertex_count)
{
  if (vertex_count > 0)
  {
    dvb.vb.draw(vertex_count, PrimitiveDrawMode<PrimitiveT>(), 5.0);
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
    if (!scene.active_camera.has_value())
    {
      return;
    }

    primitives_shader_.bind();

    if (scene.graphics.any_of<TopDownCamera2D>(*scene.active_camera))
    {
      const auto SetVertexFrom2D = [](auto& dst, const auto& src) {
        dst.template head<2>() = src;
        dst[2] = 1.f;
      };
      const auto& camera = scene.graphics.get<TopDownCamera2D>(*scene.active_camera);
      const auto camera_matrix = ToCameraMatrix(camera);
      primitives_shader_.setMat3("uCameraTransform", camera_matrix.data());
      {
        std::size_t vertex_count = 0;
        vertex_count = SubmitPrimitives<LineList2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
        DrawPrimitives<LineList2D>(primitives_vb_, vertex_count);
      }
      {
        std::size_t vertex_count = 0;
        vertex_count = SubmitPrimitives<LineStrip2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
        vertex_count = SubmitRect2fAsLineStrips(primitives_vb_, scene.graphics, vertex_count);
        DrawPrimitives<LineStrip2D>(primitives_vb_, vertex_count);
      }
      {
        std::size_t vertex_count = 0;
        vertex_count = SubmitPrimitives<Points2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
        DrawPrimitives<Points2D>(primitives_vb_, vertex_count);
      }
    };
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