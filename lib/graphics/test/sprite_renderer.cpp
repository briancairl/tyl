/**
 * @copyright 2022-present Brian Cairl
 *
 * @file sprite_renderer.cpp
 */

// C++ Standard Library
#include <cstdio>
#include <exception>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/ecs/ecs.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/graphics/sprite/animation.hpp>
#include <tyl/graphics/sprite/spritesheet.hpp>
#include <tyl/graphics/systems/render_target.hpp>
#include <tyl/graphics/systems/renderable.hpp>
#include <tyl/graphics/systems/sprite_renderer.hpp>
#include <tyl/math/size.hpp>
#include <tyl/math/vec.hpp>

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
  GLFWwindow* window = glfwCreateWindow(x_size, y_size, "sprite-renderer-test", NULL, NULL);

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

  tyl::ecs::registry reg;

  // Upload texture data
  {
    const auto texture_guid = reg.create();
    const auto texture_host = host::load("lib/graphics/test/texture-image.png");
    const auto& texture = reg.emplace<device::Texture>(texture_guid, texture_host);

    systems::create_sprite_renderer(reg, {texture_guid, texture}, {.capacity = 1000, .atlas_texture_unit = 0});
  }

  // Sprite
  {
    const auto e = reg.create();

    reg.emplace<systems::tags::rendering_enabled>(e);
    reg.emplace<tyl::Vec2f>(e, 0.f, 0.f);
    reg.emplace<tyl::Size2f>(e, 0.5f, 0.5f);
    reg.emplace<sprite::AnimationFrames>(
      e, sprite::AnimationFrames{{{0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.5f, 0.5f}}});
    reg.emplace<sprite::AnimationState>(e);
    reg.emplace<sprite::AnimationProperties>(e, 0.5f);
  }

  systems::RenderTarget2D render_target;
  render_target.view_projection.setIdentity();

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    systems::update_sprite_renderers(reg, render_target);

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }

  return 0;
}
