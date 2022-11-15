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
#include <tyl/graphics/common.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/graphics/render_target.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/sprite_animation.hpp>
#include <tyl/graphics/sprite_rendering.hpp>
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

  tyl::ecs::registry reg;

  const auto atlas_texture = tyl::graphics::create_texture(
    reg, {.path = "systems/graphics/test/sprite_renderer/spritesheet.png", .flip_vertically = false});

  tyl::graphics::create_sprite_renderer(reg, atlas_texture, {.capacity = 1000, .atlas_texture_unit = 0});

  // Sprite 1
  {
    const auto e = tyl::graphics::create_sprite(
      reg,
      {0.f, 0.f},
      {1.f, 1.f},
      {.atlas_texture_size_px = tyl::ecs::get<tyl::graphics::TextureSize>(reg, atlas_texture).get(),
       .subdivisions = {4, 1},
       .inner_padding_px = {0, 0},
       .area_px = {{0, 0}, {128, 48}},
       .transpose = false});

    tyl::graphics::attach_sprite_animation_repeating(reg, e, {.progress_per_second = 1.f});
    tyl::graphics::enable_rendering(reg, e);
  }

  // Sprite 2
  {
    const auto e = tyl::graphics::create_sprite(
      reg,
      {-1.f, -1.f},
      {1.f, 1.f},
      {.atlas_texture_size_px = tyl::ecs::get<tyl::graphics::TextureSize>(reg, atlas_texture).get(),
       .subdivisions = {4, 1},
       .inner_padding_px = {0, 0},
       .area_px = {{0, 0}, {128, 48}},
       .transpose = false});

    tyl::graphics::attach_sprite_animation_one_shot(reg, e, {.progress_per_second = 1.f});
    tyl::graphics::enable_rendering(reg, e);
  }

  tyl::graphics::RenderTarget2D render_target;
  render_target.view_projection.setIdentity();

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    tyl::graphics::update_sprite_renderers(reg, render_target);
    tyl::graphics::update_sprite_animations(reg, 0.01f);

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }

  return 0;
}
