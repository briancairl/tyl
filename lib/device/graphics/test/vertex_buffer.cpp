/**
 * @copyright 2021-present Brian Cairl
 *
 * @file vertex_buffer.cpp
 */

//
// A simple test program which draws triangles to the screen, testing:
// - VertexBuffer and companion objects
// - Shared and companion objects
//

// C++ Standard Library
#include <cstdio>
#include <exception>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/device/graphics/debug.hpp>
#include <tyl/device/graphics/shader.hpp>
#include <tyl/device/graphics/vertex_buffer.hpp>

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

  using namespace tyl::graphics::device;

  static constexpr std::size_t VERTEX_COUNT = 3;

  auto [vb, positions, colors] = VertexBuffer::create(
    VertexBuffer::BufferMode::Static, VertexAttribute<float, 2>{VERTEX_COUNT}, VertexAttribute<float, 4>{VERTEX_COUNT});

  {
    auto mapped = vb.get_mapped_vertex_buffer();

    mapped.access(positions, [](auto* const data) {
      data[0] = -0.5f;
      data[1] = -0.5f;

      data[2] = +0.5f;
      data[3] = -0.5f;

      data[4] = +0.0f;
      data[5] = +0.5f;
    });

    mapped.access(colors, [](auto* const data) {
      data[0] = +1.0f;
      data[1] = +0.0f;
      data[2] = +0.0f;
      data[3] = +1.0f;

      data[4] = +0.0f;
      data[5] = +1.0f;
      data[6] = +0.0f;
      data[7] = +1.0f;

      data[8] = +0.0f;
      data[9] = +0.0f;
      data[10] = +1.0f;
      data[11] = +1.0f;
    });
  }


  auto [e_vb, e_elements, e_positions, e_colors] = VertexElementBuffer::create(
    VertexBuffer::BufferMode::Static,
    3UL,
    VertexAttribute<float, 2>{VERTEX_COUNT},
    VertexAttribute<float, 4>{VERTEX_COUNT});

  {
    auto mapped = e_vb.get_mapped_element_buffer();

    auto* p = mapped(e_elements);

    p[0] = 0;
    p[1] = 1;
    p[2] = 2;
  }

  {
    auto mapped = e_vb.get_mapped_vertex_buffer();

    mapped.access(positions, [](auto* const data) {
      data[0] = -0.25f;
      data[1] = -0.25f;

      data[2] = +0.25f;
      data[3] = -0.25f;

      data[4] = +0.0f;
      data[5] = +0.25f;
    });

    mapped.access(colors, [](auto* const data) {
      data[0] = +0.0f;
      data[1] = +0.0f;
      data[2] = +1.0f;
      data[3] = +1.0f;

      data[4] = +0.0f;
      data[5] = +1.0f;
      data[6] = +0.0f;
      data[7] = +1.0f;

      data[8] = +1.0f;
      data[9] = +0.0f;
      data[10] = +0.0f;
      data[11] = +1.0f;
    });
  }

  Shader shader{
    ShaderSource::vertex(
      R"VertexShader(

      layout (location = 0) in vec2 vPos;
      layout (location = 1) in vec4 vColor;

      out vec4 vFragColor;

      void main()
      {
        gl_Position = vec4(vPos, 0, 1);
        vFragColor = vColor;
      }

      )VertexShader"),
    ShaderSource::fragment(
      R"FragmentShader(

      out vec4 FragColor;

      in vec4 vFragColor;

      void main()
      {
        FragColor = vFragColor;
      }

      )FragmentShader")};

  shader.bind();

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    vb.draw(positions, VertexBuffer::DrawMode::Triangles);
    e_vb.draw(e_elements, VertexBuffer::DrawMode::Triangles);

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }

  return 0;
}
