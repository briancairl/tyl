/**
 * @copyright 2022-present Brian Cairl
 *
 * @file texture.cpp
 */

//
// A simple test program which draws VBO colored with a texture
// Also downloads/re-uploads texture data
//

// C++ Standard Library
#include <cstdio>
#include <cstring>
#include <exception>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/graphics/image.hpp>

static void glfw_error_callback(int error, const char* description)
{
  std::fprintf(stderr, "%d : %s\n", error, description);
}


int main(int argc, char** argv)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
  {
    std::terminate();
  }

  int x_size = 500;
  int y_size = 500;

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(x_size, y_size, "vertex-buffer-test", NULL, NULL);

  if (window == NULL)
  {
    std::terminate();
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);  // Enable vsync
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  tyl::graphics::device::enable_debug_logs();
  tyl::graphics::device::enable_error_logs();

  using namespace tyl::graphics;
  using namespace tyl::graphics::device;

  auto image = load(Image{"lib/graphics/test/texture-image.png"});

  // Upload texture data
  const Texture texture{image};

  texture.bind(1);

  static constexpr std::size_t VERTEX_COUNT = 4;

  auto [vb, elements, positions, texcoords] = VertexElementBuffer::create(
    VertexBuffer::BufferMode::Static,
    6UL,
    VertexAttribute<float, 2>{VERTEX_COUNT},
    VertexAttribute<float, 2>{VERTEX_COUNT});

  {
    auto mapped = vb.get_mapped_element_buffer();

    {
      auto* const p = mapped(elements);

      p[0] = 0;
      p[1] = 1;
      p[2] = 2;

      p[3] = 1;
      p[4] = 2;
      p[5] = 3;
    }
  }

  {
    auto mapped = vb.get_mapped_vertex_buffer();

    {
      auto* const p = mapped(positions);

      p[0] = -0.25f;
      p[1] = -0.25f;

      p[2] = +0.25f;
      p[3] = -0.25f;

      p[4] = -0.25f;
      p[5] = +0.25f;

      p[6] = +0.25f;
      p[7] = +0.25f;
    }

    {
      auto* const p = mapped(texcoords);

      p[0] = +0.0f;
      p[1] = +0.0f;

      p[2] = +1.0f;
      p[3] = +0.0f;

      p[4] = +0.0f;
      p[5] = +1.0f;

      p[6] = +1.0f;
      p[7] = +1.0f;
    }
  }

  Shader shader{
    ShaderSource::vertex(
      R"VertexShader(

      layout (location = 0) in vec2 vPos;
      layout (location = 1) in vec2 vTexCoord;

      out vec2 fTexCoord;

      void main()
      {
        gl_Position = vec4(vPos, 0, 1);
        fTexCoord = vTexCoord;
      }

      )VertexShader"),
    ShaderSource::fragment(
      R"FragmentShader(

      out vec4 FragColor;

      in vec2 fTexCoord;

      uniform sampler2D fTextureID;

      void main()
      {
        FragColor = texture(fTextureID, fTexCoord);
      }

      )FragmentShader")};

  shader.bind();
  shader.setInt("fTextureID", 1);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    vb.draw(elements, VertexBuffer::DrawMode::Triangles);

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }

  return 0;
}
