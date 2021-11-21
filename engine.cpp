// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/engine/app.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/vertex_buffer.hpp>
#include <tyl/render/camera.hpp>

// ImGui
#include <imgui.h>

int main(int argc, char** argv)
{
  tyl::engine::App app{"tyl", tyl::render::ViewportSize{720, 720}};

  using namespace tyl::graphics;

  using Vec2f = Eigen::Matrix<float, 2, 1>;

  VertexBuffer new_mesh{6UL,
                        {
                          VertexAttributeDescriptor{TypeCode::Float32, 2, 4, 0},  // vertex position
                          VertexAttributeDescriptor{TypeCode::Float32, 2, 4, 0},  // texture quad extents
                        },
                        VertexBuffer::BufferMode::STATIC};

  {
    const unsigned indices[] = {0, 1, 2, 2, 3, 0};
    new_mesh.set_index_data(indices);
  }

  {
    const Vec2f points[] = {
      Vec2f{-0.1f, -0.1f},
      Vec2f{+0.1f, -0.1f},
      Vec2f{+0.1f, +0.1f},
      Vec2f{-0.1f, +0.1f},
    };
    new_mesh.set_vertex_data(0, reinterpret_cast<const float*>(points));
  }

  {
    const Vec2f points[] = {
      Vec2f{0.f, 0.f},
      Vec2f{1.f, 0.f},
      Vec2f{1.f, 1.f},
      Vec2f{0.f, 1.f},
    };
    new_mesh.set_vertex_data(1, reinterpret_cast<const float*>(points));
  }

  const Texture texture{Image::load_from_file("/home/brian/Downloads/smol_bilal_cat.png")};
  texture.bind(0);

  const Shader shader{ShaderSource{get_shader_version_preamble() +
                                     R"VertexShader(

      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTexCoord;

      uniform mat3 uModel;
      uniform mat3 uView;

      out vec2 vTexCoord;

      void main()
      {
        gl_Position =  vec4(uView * uModel * vec3(aPos, 1), 1);
        vTexCoord = aTexCoord;
      }

      )VertexShader",
                                   ShaderType::VERTEX},
                      ShaderSource{get_shader_version_preamble() +
                                     R"FragmentShader(

      out vec4 FragColor;

      in vec2 vTexCoord;

      uniform sampler2D uTextureID;

      void main()
      {
        FragColor = texture(uTextureID, vTexCoord);
      }

      )FragmentShader",
                                   ShaderType::FRAGMENT}};

  entt::registry registry;

  registry.set<tyl::render::TopDownCamera>();

  return app.run([&](const tyl::render::ViewportSize& window_size) -> bool {
    auto& camera = registry.ctx<tyl::render::TopDownCamera>();
    ImGui::Begin("camera");

    ImGui::SliderFloat("zoom", &camera.zoom, 0.1f, 10.f);
    ImGui::SliderFloat("panning.x", &camera.panning[0], -10.f, 10.f);
    ImGui::SliderFloat("panning.y", &camera.panning[1], -10.f, 10.f);

    const auto view_matrix = tyl::render::get_view_matrix(camera, window_size);

    ImGui::Text(
      "[%8.3f, %8.3f, %8.3f]\n"
      "[%8.3f, %8.3f, %8.3f]\n"
      "[%8.3f, %8.3f, %8.3f]\n",
      view_matrix(0, 0),
      view_matrix(0, 1),
      view_matrix(0, 2),
      view_matrix(1, 0),
      view_matrix(1, 1),
      view_matrix(1, 2),
      view_matrix(2, 0),
      view_matrix(2, 1),
      view_matrix(2, 2));

    ImGui::End();

    const tyl::Mat3f model{tyl::Mat3f::Identity()};

    shader.bind();
    shader.setMat3("uView", view_matrix.data());
    shader.setMat3("uModel", model.data());
    shader.setMat3("uTextureID", 0);
    new_mesh.draw();

    return true;
  });
}
