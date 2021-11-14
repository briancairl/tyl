// C++ Standard Library

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// OpenGL
#include <GL/gl3w.h>  // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Tyl
#include <tyl/ui/style.hpp>

/**
 * @brief Logging callback for when glfw shits a brick
 */
static void glfw_error_callback(int error, const char* description) {}


int main(int argc, char** argv)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

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

  // Create window with graphics context
  int display_w = 720, display_h = 720;
  GLFWwindow* window = glfwCreateWindow(display_w, display_h, "tyl", NULL, NULL);
  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  if (gl3wInit() != 0)
  {
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImGuiIO& imgui_io = ImGui::GetIO();
  // ImGuiStyle& imgui_style = ImGui::GetStyle();

  // Setup Dear ImGui style
  tyl::ui::InitStyleDefault(&ImGui::GetStyle());

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ImVec4 bg_color{0.1f, 0.1f, 0.1f, 1.0f};

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowStyleEditor(&imgui_style);
    // ImGui::ShowDemoWindow();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
