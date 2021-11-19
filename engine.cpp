// Tyl
#include <tyl/engine/app.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/vertex_buffer.hpp>

#include <Eigen/Dense>


int main(int argc, char** argv)
{
  tyl::engine::App app{"tyl", tyl::engine::WindowSize{720, 720}};

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
      Vec2f{0.0f, 0.0f},
      Vec2f{0.1f, 0.0f},
      Vec2f{0.1f, 0.1f},
      Vec2f{0.0f, 0.1f},
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

  const Texture tex{Image::load_from_file("/home/brian/Downloads/smol_bilal_cat.png")};

  return app.run([&](const tyl::engine::WindowSize& window_size) -> bool {
    new_mesh.draw();
    return true;
  });
}
