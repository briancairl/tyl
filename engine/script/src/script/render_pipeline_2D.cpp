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
uniform mat4 uCameraTransform;

void main()
{
  gl_Position = uCameraTransform * vec4(vPos, 1);
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

std::size_t SubmitRectsAsLineList(PrimitivesVertexBuffer& dvb, const Registry& registry, std::size_t vertex_count)
{
  auto view = registry.view<Rect2D, Color>();
  {
    auto mapped = dvb.vb.get_mapped_vertex_buffer();
    auto* const position_ptr = reinterpret_cast<tyl::Vec3f*>(mapped(dvb.position));
    auto* const color_ptr = reinterpret_cast<tyl::Vec4f*>(mapped(dvb.color));

    auto add_vertex =
      [position_ptr, color_ptr, &vertex_count](const float x, const float y, const Vec4f& color) mutable {
        position_ptr[vertex_count] << x, y, 0;
        color_ptr[vertex_count] = color;
        ++vertex_count;
      };

    for (const auto e : view)
    {
      static constexpr std::size_t kPoints = 4;
      static constexpr std::size_t kVerticesAdded = 2 * kPoints;

      const auto& rect = view.template get<Rect2D>(e);
      const auto& color = view.template get<Color>(e);

      // Stop adding vertices if we will go past the max vertex count
      if (vertex_count + kVerticesAdded > dvb.max_vertex_count)
      {
        break;
      }

      add_vertex(rect.min().x(), rect.min().y(), color.rgba);
      add_vertex(rect.min().x(), rect.max().y(), color.rgba);

      add_vertex(rect.min().x(), rect.max().y(), color.rgba);
      add_vertex(rect.max().x(), rect.max().y(), color.rgba);

      add_vertex(rect.max().x(), rect.max().y(), color.rgba);
      add_vertex(rect.max().x(), rect.min().y(), color.rgba);

      add_vertex(rect.max().x(), rect.min().y(), color.rgba);
      add_vertex(rect.min().x(), rect.min().y(), color.rgba);
    }
  }
  return vertex_count;
}

template <typename PrimitiveT> void DrawPrimitives(PrimitivesVertexBuffer& dvb, std::size_t vertex_count)
{
  if (vertex_count > 0)
  {
    dvb.vb.draw(vertex_count, PrimitiveDrawMode<PrimitiveT>(), 1.0);
  }
}

static constexpr const char* kSpriteVertexShaderSource =
  R"VertexShader(

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

out vec4 vFragColor;
uniform mat4 uCameraTransform;

void main()
{
  gl_Position = uCameraTransform * vec4(vPos, 1);
  vFragColor = vColor;
}

)VertexShader";

static constexpr const char* kSpriteFragmentShaderSource =
  R"FragmentShader(

layout(location = 0) out vec4 FragColor;

in vec4 vFragColor;

void main()
{
  FragColor = vFragColor;
}

)FragmentShader";

struct SpriteVertexBuffer
{
  using Position = VertexAttribute<float, 2>;
  using UVCoord = VertexAttribute<float, 2>;

  VertexAttributeBuffer<float> position;
  VertexAttributeBuffer<float> uv_coord;

  VertexBuffer vb;

  std::size_t max_vertex_count;

  static SpriteVertexBuffer create(const std::size_t max_vertex_count)
  {
    auto [vb, position, uv_coord] =
      VertexBuffer::create(VertexBuffer::BufferMode::kDynamic, Position{max_vertex_count}, UVCoord{max_vertex_count});

    return {
      .position = std::move(position),
      .uv_coord = std::move(uv_coord),
      .vb = std::move(vb),
      .max_vertex_count = max_vertex_count};
  }
};

// void DrawTileMaps()

}  // namespace

class RenderPipeline2D::Impl
{
public:
  Impl(
    Shader&& primitives_shader,
    PrimitivesVertexBuffer&& primitives_vb,
    Shader&& sprite_shader,
    SpriteVertexBuffer&& sprite_vertex_buffer) :
      primitives_shader_{std::move(primitives_shader)},
      primitives_vb_{std::move(primitives_vb)},
      sprite_shader_{std::move(sprite_shader)},
      sprite_vb_{std::move(sprite_vertex_buffer)}
  {}

  void Update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    if (!scene.active_camera.has_value())
    {
      return;
    }

    if (scene.graphics.any_of<TopDownCamera2D>(*scene.active_camera))
    {
      const auto& camera = scene.graphics.get<TopDownCamera2D>(*scene.active_camera);
      const auto camera_matrix = ToCameraMatrix(camera);
      RenderPrimitives(scene, camera_matrix);
    };
  }

private:
  void RenderPrimitives(Scene& scene, const Mat4f& camera_matrix)
  {
    const auto SetVertexFrom2D = [](auto& dst, const auto& src) {
      dst.template head<2>() = src;
      dst[2] = 0.f;
    };

    primitives_shader_.bind();
    primitives_shader_.setMat4("uCameraTransform", camera_matrix.data());

    {
      std::size_t vertex_count = 0;
      vertex_count = SubmitPrimitives<LineList2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
      vertex_count = SubmitRectsAsLineList(primitives_vb_, scene.graphics, vertex_count);
      DrawPrimitives<LineList2D>(primitives_vb_, vertex_count);
    }
    {
      std::size_t vertex_count = 0;
      vertex_count = SubmitPrimitives<LineStrip2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
      DrawPrimitives<LineStrip2D>(primitives_vb_, vertex_count);
    }
    {
      std::size_t vertex_count = 0;
      vertex_count = SubmitPrimitives<Points2D>(primitives_vb_, scene.graphics, SetVertexFrom2D, vertex_count);
      DrawPrimitives<Points2D>(primitives_vb_, vertex_count);
    }
  }

  Shader primitives_shader_;

  PrimitivesVertexBuffer primitives_vb_;

  Shader sprite_shader_;

  SpriteVertexBuffer sprite_vb_;
};

RenderPipeline2D::~RenderPipeline2D() = default;

tyl::expected<RenderPipeline2D, ScriptCreationError>
RenderPipeline2D::CreateImpl(const RenderPipeline2DOptions& options)
{
  if (auto primitives_vertex_shader = ShaderSource::vertex(kPrimitivesVertexShaderSource); !primitives_vertex_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto primitives_fragment_shader = ShaderSource::fragment(kPrimitivesFragmentShaderSource);
           !primitives_fragment_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto primitives_shader = Shader::create(*primitives_vertex_shader, *primitives_fragment_shader);
           !primitives_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto sprite_vertex_shader = ShaderSource::vertex(kSpriteVertexShaderSource); !sprite_vertex_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto sprite_fragment_shader = ShaderSource::fragment(kSpriteFragmentShaderSource); !sprite_fragment_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else if (auto sprite_shader = Shader::create(*sprite_vertex_shader, *sprite_fragment_shader); !sprite_shader)
  {
    return tyl::unexpected<ScriptCreationError>{ScriptCreationError::kInternalSetupFailure};
  }
  else
  {
    return RenderPipeline2D{
      options,
      std::make_unique<Impl>(
        std::move(primitives_shader).value(),
        PrimitivesVertexBuffer::create(options.max_vertex_count),
        std::move(sprite_shader).value(),
        SpriteVertexBuffer::create(options.max_vertex_count))};
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