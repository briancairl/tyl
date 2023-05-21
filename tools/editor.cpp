/**
 * @copyright 2022-present Brian Cairl
 *
 * @file editor.cpp
 */


// C++ Standard Library
#include <cstdio>
#include <iostream>
#include <optional>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// ImGui
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/texture.hpp>

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

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

  int x_size = 2000;
  int y_size = 1000;

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(x_size, y_size, "editor", NULL, NULL);

  TYL_ASSERT_NON_NULL(window);

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::terminate();
  }
  glfwSwapInterval(1);  // Enable vsync

  // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

  tyl::graphics::device::enable_debug_logs();
  tyl::graphics::device::enable_error_logs();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("editor", nullptr, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }


  return 0;
}
