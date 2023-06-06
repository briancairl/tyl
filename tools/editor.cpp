/**
 * @copyright 2022-present Brian Cairl
 *
 * @file editor.cpp
 */


// C++ Standard Library
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>
#include <unordered_map>

// GLAD
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/utility/expected.hpp>

using namespace tyl::graphics;

static void glfw_error_callback(int error, const char* description)
{
  std::fprintf(stderr, "%d : %s\n", error, description);
}

struct DefaultReferenceResolver
{
  template <typename T> constexpr T& operator()(T* p) const { return *p; }
};

using reference_count_type = std::size_t;

template <typename ReferenceT, typename ManagerT, typename ReferenceResolverT = DefaultReferenceResolver>
class ReferenceCounted
{
public:
  ReferenceCounted() = default;

  ReferenceCounted(const ReferenceCounted& other) :
      ref_count_{other.ref_count_}, ref_{other.ref_}, ref_resolver_{other.ref_resolver_}
  {
    if (ref_count_ != nullptr)
    {
      ++(*ref_count_);
    }
  }

  ReferenceCounted(ReferenceCounted&& other) :
      ref_count_{other.ref_count_}, ref_{std::move(other.ref_)}, ref_resolver_{std::move(other.ref_resolver_)}
  {
    other.ref_count_ = nullptr;
  }

  ~ReferenceCounted()
  {
    if (ref_count_ == nullptr)
    {
      return;
    }
    else
    {
      --(*ref_count_);
    }
  }

  decltype(auto) operator*() { return ref_resolver_(ref_); }

  decltype(auto) operator*() const { return ref_resolver_(ref_); }

  void reset() { ref_count_ = nullptr; }

  bool valid() const { return ref_count_ != nullptr; }

  operator bool() const { return valid(); }

  reference_count_type use_count() const { return *ref_count_; }

private:
  friend ManagerT;

  ReferenceCounted(ReferenceT ref, reference_count_type* const ref_count, ReferenceResolverT ref_resolver = {}) :
      ref_count_{ref_count}, ref_{ref}, ref_resolver_{ref_resolver}
  {}

  reference_count_type* ref_count_ = nullptr;
  ReferenceT ref_;
  ReferenceResolverT ref_resolver_;
};


class TextureManager
{
public:
  using reference_type = ReferenceCounted<const device::Texture*, TextureManager>;

  [[nodiscard]] reference_type
  get(const std::filesystem::path& texture_path, const host::ImageOptions& options = {}, const bool try_reload = false)
  {
    auto [itr, was_added] = cache_.try_emplace(texture_path);

    if (itr->second.texture.has_value())
    {
      // already populated
    }
    else if (!was_added and !try_reload)
    {
      return reference_type{};
    }
    else if (auto image_or_error = host::Image::load(texture_path.string().c_str(), options);
             image_or_error.has_value())
    {
      itr->second.texture.emplace(image_or_error->texture());
    }
    return reference_type{std::addressof(*itr->second.texture), std::addressof(itr->second.use_count)};
  }

  [[nodiscard]] reference_type get(const std::filesystem::path& texture_path) const
  {
    if (auto itr = cache_.find(texture_path); itr == cache_.end() or itr->second.texture)
    {
      return reference_type{};
    }
    else
    {
      return reference_type{std::addressof(*itr->second.texture), std::addressof(itr->second.use_count)};
    }
  }

  template <typename ObserverT> void for_each(ObserverT observer) const
  {
    for (const auto& [path, texture_data] : cache_)
    {
      if (texture_data.texture.has_value())
      {
        observer(path, texture_data.texture);
      }
    }
  }

  template <typename OnRemoveT> void prune(OnRemoveT on_remove)
  {
    for (auto itr = cache_.begin(); itr != cache_.end(); /*empty*/)
    {
      if (auto& [path, data] = *itr; data.use_count == 0)
      {
        on_remove(path);
        itr = cache_.erase(itr);
      }
      else
      {
        ++itr;
      }
    }
  }

  void prune()
  {
    prune([]([[maybe_unused]] const auto& _) {});
  }

private:
  struct TextureData
  {
    std::optional<device::Texture> texture;
    mutable std::size_t use_count = 0;
  };

  struct HashPath
  {
    std::size_t operator()(const std::filesystem::path& p) const { return std::filesystem::hash_value(p); }
  };

  std::unordered_map<std::filesystem::path, TextureData, HashPath> cache_;
};


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

  device::enable_debug_logs();
  device::enable_error_logs();

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

  TextureManager textures;

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("editor", nullptr, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse));

    if (ImGui::Button("Open File Dialog"))
    {
      ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureSource", "Choose File", ".png,.jpg", ".");
    }

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseTextureSource"))
    {
      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        const std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
        [[maybe_unused]] const auto _ = textures.get(file_path_name.c_str());
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }

    textures.for_each([](const auto& path, const auto& texture) { ImGui::Text("%s", path.string().c_str()); });

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }


  return 0;
}
