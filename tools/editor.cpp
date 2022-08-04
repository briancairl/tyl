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
#include <tyl/common/alias.hpp>
#include <tyl/common/assert.hpp>
#include <tyl/common/dynamic_bitset.hpp>
#include <tyl/common/reference.hpp>
#include <tyl/common/vec.hpp>
#include <tyl/graphics/animated_sprite.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/texture_tilesheet_lookup.hpp>

static void glfw_error_callback(int error, const char* description)
{
  std::fprintf(stderr, "%d : %s\n", error, description);
}

static ImVec2 transform(const tyl::Mat3f& t, const ImVec2& pos)
{
  const tyl::Vec3f augmented{pos.x, pos.y, 1.f};
  const tyl::Vec3f result{t * augmented};
  return ImVec2{result.x(), result.y()};
}

static ImVec2 round(const ImVec2& pos) { return ImVec2(std::lround(2 * pos.x) / 2, std::lround(2 * pos.y) / 2); }

static bool within(const ImVec2& delta, const float radius)
{
  return ((delta.x * delta.x) + (delta.y * delta.y)) < (radius * radius);
}

struct TextureDisplayState
{
  entt::entity entity;
  tyl::Vec2i extents;
  float zoom_level;
  ImVec2 origin;
  std::optional<ImVec2> origin_drag_start;
};

struct RegionEditState
{
  ImVec2 rect_min;
  ImVec2 rect_max;
  ImVec2* editting_point = nullptr;
  tyl::Vec2i subdivisions;
  // tyl::graphics::UniformlyDividedRegion region;
};

using AnimatedSpriteFramesPallet = tyl::Alias<tyl::graphics::AnimatedSpriteFrames, std::size_t, 0>;
using TextureHandle = tyl::Reference<entt::entity, tyl::graphics::device::TextureHandle>;

static void draw(
  ImDrawList* const drawlist,
  const tyl::Mat3f& screen_to_local,
  const RegionEditState& region,
  const bool show_details = true)
{
  const ImVec2 rect_min_screen{transform(screen_to_local, region.rect_min)};
  const ImVec2 rect_max_screen{transform(screen_to_local, region.rect_max)};
  const ImVec2 extents{rect_max_screen - rect_min_screen};
  const ImVec2 extents_divisions{extents.x / region.subdivisions.x(), extents.y / region.subdivisions.y()};

  drawlist->AddRect(rect_min_screen, rect_max_screen, IM_COL32(255, 255, 255, 255), 0.f, 0, 2.f);

  for (int x = 0; x < region.subdivisions.x(); ++x)
  {
    for (int y = 0; y < region.subdivisions.y(); ++y)
    {
      const ImVec2 sub_rect_min{rect_min_screen + ImVec2{(x + 0) * extents_divisions.x, (y + 0) * extents_divisions.y}};
      const ImVec2 sub_rect_max{rect_min_screen + ImVec2{(x + 1) * extents_divisions.x, (y + 1) * extents_divisions.y}};
      drawlist->AddRect(sub_rect_min, sub_rect_max, IM_COL32(255, 255, 255, 255), 0.f, 0, 2.f);
    }
  }

  drawlist->AddCircleFilled(rect_min_screen, 2.f, IM_COL32(255, 0, 255, 255));
  drawlist->AddCircleFilled(rect_max_screen, 2.f, IM_COL32(255, 0, 255, 255));

  if (show_details)
  {
    const ImVec2 image_extents{
      std::abs(region.rect_min.x - region.rect_max.x), std::abs(region.rect_min.y - region.rect_max.y)};
    static char text_buffer[32];
    std::snprintf(text_buffer, sizeof(text_buffer), "%f x %f\n", image_extents.x, image_extents.y);
    drawlist->AddText(rect_max_screen + ImVec2{10, 10}, IM_COL32(255, 255, 255, 255), text_buffer);
  }
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

  if (window == NULL)
  {
    std::terminate();
  }

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

  using namespace tyl::graphics;

  entt::registry reg;

  ImVec2* editting_point = nullptr;
  std::optional<entt::entity> active_new_region_id;
  std::optional<entt::entity> active_editting_region_id;
  std::optional<TextureDisplayState> edittor_state;

  bool disable_window_move = false;
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin(
      "editor-test", nullptr, disable_window_move * (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse));

    static char filename_buffer[128] = "/home/brian/dev/tyl/resources/test/poke-npc-walk.png";
    if (ImGui::InputText("file", filename_buffer, sizeof(filename_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
      try
      {
        auto image_data = load(Image{filename_buffer});
        const auto parent_e = reg.create();
        const auto& texture = reg.emplace<device::Texture>(parent_e, image_data);

        const auto e = reg.create();
        reg.emplace<TextureHandle>(e, parent_e, texture);
        reg.emplace<tyl::Vec2i>(e, image_data.height(), image_data.width());
      }
      catch (const std::runtime_error& ex)
      {
        std::fprintf(stderr, "%s\n", ex.what());
      }
    }


    ImGui::BeginChild("#textures", ImVec2{0, 200}, true);
    {
      auto* const drawlist = ImGui::GetWindowDrawList();
      auto view = reg.view<const TextureHandle, const tyl::Vec2i>();
      view.each(
        [drawlist, &reg, &edittor_state](const auto entity, const TextureHandle& texture, const tyl::Vec2i& extents) {
          {
            const ImTextureID im_texture_id = reinterpret_cast<ImTextureID>(texture.value().get_id());
            const float ratio = static_cast<float>(extents.y()) / static_cast<float>(extents.x());
            ImGui::Image(im_texture_id, ImVec2{200, ratio * 200});
          }

          const auto rect_min = ImGui::GetItemRectMin();
          const auto rect_max = ImGui::GetItemRectMax();

          if (edittor_state.has_value() and (edittor_state->entity == entity))
          {
            static constexpr float rounding = 0.0f;
            static constexpr int flags = 0;
            static constexpr float thickness = 3.0f;
            drawlist->AddRect(rect_min, rect_max, IM_COL32(255, 0, 255, 255), rounding, flags, thickness);
          }
          else if (ImGui::IsMouseHoveringRect(rect_min, rect_max))
          {
            if (
              ImGui::IsMouseDown(ImGuiMouseButton_Left) and
              (!edittor_state.has_value() or edittor_state->entity != entity))
            {
              static constexpr float rounding = 0.0f;
              static constexpr int flags = 0;
              static constexpr float thickness = 2.0f;
              drawlist->AddRect(rect_min, rect_max, IM_COL32(255, 255, 0, 255), rounding, flags, thickness);

              edittor_state = TextureDisplayState{
                .entity = entity,
                .extents = extents,
                .zoom_level = 1.f,
                .origin = ImVec2{0, 0},
                .origin_drag_start = std::nullopt};
            }
            else
            {
              static constexpr float rounding = 0.0f;
              static constexpr int flags = 0;
              static constexpr float thickness = 2.0f;
              drawlist->AddRect(rect_min, rect_max, IM_COL32(255, 0, 0, 255), rounding, flags, thickness);
            }
          }

          ImGui::SameLine();
          ImGui::PushID(static_cast<int>(entity));
          if (ImGui::SmallButton("delete"))
          {
            reg.destroy(entity);
            if (edittor_state.has_value() and entity == edittor_state->entity)
            {
              edittor_state.reset();
            }
          }
          ImGui::PopID();

          ImGui::SameLine();
          ImGui::Text("%d x %d", extents.x(), extents.y());
        });
    }
    ImGui::EndChild();

    static constexpr float MIN_ZOOM = 0.1f;
    static constexpr float MAX_ZOOM = 10.f;
    static constexpr float ZOOM_SCALING = 0.1f;

    if (edittor_state.has_value())
    {
      ImGui::SliderFloat("zoom", &edittor_state->zoom_level, MIN_ZOOM, MAX_ZOOM);
      if (ImGui::SmallButton("reset panning"))
      {
        edittor_state->origin = ImVec2{0, 0};
      }
    }

    ImGui::BeginChild("#texture", ImVec2{0, 0}, true, ImGuiWindowFlags_HorizontalScrollbar);
    if (!edittor_state.has_value() or active_new_region_id.has_value())
    {
      // trap
    }
    else if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
      if (!edittor_state->origin_drag_start.has_value())
      {
        edittor_state->origin_drag_start = edittor_state->origin;
      }
      else
      {
        const auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
        edittor_state->origin = edittor_state->origin_drag_start.value() + delta;
      }
    }
    else
    {
      edittor_state->origin_drag_start.reset();
      edittor_state->zoom_level =
        std::max(MIN_ZOOM, std::min(edittor_state->zoom_level + ZOOM_SCALING * ImGui::GetIO().MouseWheel, MAX_ZOOM));
    }
    ImGui::EndChild();


    if (edittor_state.has_value())
    {
      const auto texture_display_rect_min = ImGui::GetItemRectMin();
      const auto texture_display_rect_max = ImGui::GetItemRectMax();
      disable_window_move = ImGui::IsMouseHoveringRect(texture_display_rect_min, texture_display_rect_max);

      // clang-format off
      const tyl::Mat3f screen_to_local{
        [&p=texture_display_rect_min, &edittor=edittor_state.value()]
        {
          tyl::Mat3f m;
          m << edittor.zoom_level, 0,                  p.x + edittor.origin.x,
               0,                  edittor.zoom_level, p.y + edittor.origin.y,
               0,                  0,                  1                      
               ;
          return m;
        }()
      };
      const tyl::Mat3f local_to_screen{screen_to_local.inverse()};
      // clang-format on

      // Get mouse position in local edittor space
      const auto local_mouse_pos = transform(local_to_screen, ImGui::GetMousePos());


      auto* const drawlist = ImGui::GetForegroundDrawList();

      drawlist->PushClipRect(texture_display_rect_min, texture_display_rect_max);
      {
        const auto& texture = reg.get<TextureHandle>(edittor_state->entity);
        const ImTextureID im_texture_id = reinterpret_cast<ImTextureID>(texture().get_id());
        drawlist->AddImage(
          im_texture_id,
          transform(screen_to_local, ImVec2(0, 0)),
          transform(screen_to_local, ImVec2(edittor_state->extents.x(), edittor_state->extents.y())));
      }

      if (const auto& io = ImGui::GetIO(); !io.KeyCtrl)
      {
        // TRAP
      }
      else if (active_new_region_id.has_value())
      {
        auto& region = reg.get<RegionEditState>(active_new_region_id.value());

        region.rect_max = local_mouse_pos;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
          const auto tmp_rect_min = round(region.rect_min);
          const auto tmp_rect_max = round(region.rect_max);
          region.rect_min.x = std::min(tmp_rect_max.x, tmp_rect_min.x);
          region.rect_min.y = std::min(tmp_rect_max.y, tmp_rect_min.y);
          region.rect_max.x = std::max(tmp_rect_max.x, tmp_rect_min.x);
          region.rect_max.y = std::max(tmp_rect_max.y, tmp_rect_min.y);
          active_new_region_id.reset();
        }
      }
      else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      {
        const auto region_id = reg.create();

        active_new_region_id.emplace(region_id);

        reg.emplace<RegionEditState>(region_id) = RegionEditState{
          .rect_min = round(local_mouse_pos), .rect_max = round(local_mouse_pos), .subdivisions = {1, 1}};
      }

      if (editting_point != nullptr)
      {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
          (*editting_point) = round(local_mouse_pos);

          drawlist->AddCircle(transform(screen_to_local, *editting_point), 10.f, IM_COL32(255, 255, 0, 255));
        }
        else
        {
          drawlist->AddCircle(transform(screen_to_local, *editting_point), 10.f, IM_COL32(255, 0, 0, 255));

          editting_point = nullptr;
        }
      }

      reg.view<RegionEditState>().each(
        [drawlist,
         &reg,
         &screen_to_local,
         &local_mouse_pos,
         &editting_point,
         &active_editting_region_id,
         &edittor = edittor_state.value()](const entt::entity region_id, auto& edit_state) {
          draw(drawlist, screen_to_local, edit_state, edittor.zoom_level > 3.f);

          {
            const ImVec2 screen_rect_min = transform(screen_to_local, edit_state.rect_min);
            const ImVec2 screen_rect_max = transform(screen_to_local, edit_state.rect_max);

            if (region_id == active_editting_region_id)
            {
              drawlist->AddRectFilled(screen_rect_min, screen_rect_max, IM_COL32(255, 255, 0, 50));
            }

            if (ImGui::IsMouseHoveringRect(screen_rect_min, screen_rect_max))
            {
              if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
              {
                active_editting_region_id = region_id;
              }
            }
          }

          if (editting_point != nullptr)
          {
            return;
          }

          for (auto* editting_point_candidate : {&edit_state.rect_min, &edit_state.rect_max})
          {
            if (const auto delta = *editting_point_candidate - local_mouse_pos; !within(delta, 2.5f))
            {
              continue;
            }
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
              editting_point = editting_point_candidate;
              break;
            }
            else
            {
              drawlist->AddCircle(
                transform(screen_to_local, *editting_point_candidate), 10.f, IM_COL32(255, 255, 255, 255));
            }
          }
        });

      if (active_editting_region_id.has_value())
      {
        ImGui::Begin("editting");
        {
          auto& region = reg.get<RegionEditState>(active_editting_region_id.value());

          if (ImGui::InputInt2("subdivisions", region.subdivisions.data(), ImGuiInputTextFlags_EnterReturnsTrue))
          {
            tyl::clamp(region.subdivisions, tyl::Vec2i{1, 1}, tyl::Vec2i{50, 50});
          }

          if (ImGui::Button("unselect"))
          {
            active_editting_region_id.reset();
          }

          if (ImGui::Button("create"))
          {
            AnimatedSpriteFrames frames{
              edittor_state->extents,
              UniformlyDividedRegion{
                .subdivisions = region.subdivisions,
                .inner_padding_px = {0, 0},
                .area_px = {
                  tyl::Vec2i{region.rect_min.x, region.rect_min.y}, tyl::Vec2i{region.rect_max.x, region.rect_max.y}}}};

            auto animation_id = reg.create();
            reg.emplace<AnimatedSpriteFrames>(animation_id, std::move(frames));
            reg.emplace<AnimatedSpriteState>(animation_id, 0.f);
            reg.emplace<AnimatedSpriteProperties>(animation_id, 0.5f);

            const float dx = (region.rect_max.x - region.rect_min.x) / region.subdivisions.x();
            const float dy = (region.rect_max.y - region.rect_min.y) / region.subdivisions.y();
            reg.emplace<tyl::Vec2f>(animation_id, dx, dy);
          }

          reg.view<AnimatedSpriteState, AnimatedSpriteFrames, AnimatedSpriteProperties, tyl::Vec2f>().each(
            [&reg, &edittor_state](
              const entt::entity ani_id,
              auto& ani_state,
              const auto& ani_frames,
              const auto& ani_props,
              const auto& ani_size) {
              const auto& texture = reg.get<TextureHandle>(edittor_state->entity);
              const ImTextureID im_texture_id = reinterpret_cast<ImTextureID>(texture().get_id());
              const auto& bounds = get_frame(ani_frames, ani_state);

              tick_repeat(ani_state, ani_props, ImGui::GetIO().DeltaTime);

              ImGui::PushID(static_cast<int>(ani_id));
              ImGui::SliderFloat("progress", &ani_state.progress, 0.f, 1.f);
              ImGui::Image(
                im_texture_id,
                ImVec2{ani_size.x() * 4, ani_size.y() * 4},
                ImVec2{bounds.min()[0], bounds.min()[1]},
                ImVec2{bounds.max()[0], bounds.max()[1]});
              if (ImGui::SmallButton("delete"))
              {
                reg.destroy(ani_id);
              }
              ImGui::PopID();
            });

          if (ImGui::Button("delete"))
          {
            reg.destroy(active_editting_region_id.value());
            active_editting_region_id.reset();
          }
        }
        ImGui::End();
      }

      drawlist->PopClipRect();
    }


    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwGetFramebufferSize(window, &x_size, &y_size);
    glViewport(0, 0, x_size, y_size);
    glfwSwapBuffers(window);
  }

  return 0;
}
