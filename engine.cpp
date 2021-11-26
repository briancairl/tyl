// C++ Standard Library
#include <string>

// Tyl
#include <tyl/app/loop.hpp>
#include <tyl/camera/top_down.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/resources.hpp>
#include <tyl/tile_map/initialization.hpp>
#include <tyl/tile_map/systems.hpp>
// #include <tyl/engine/camera.hpp>
// #include <tyl/engine/geometry.hpp>
// #include <tyl/engine/tile_map.hpp>
// #include <tyl/engine/tile_map_rendering.hpp>
// #include <tyl/graphics/image.hpp>
// #include <tyl/graphics/shader.hpp>
// #include <tyl/graphics/texture.hpp>
// #include <tyl/graphics/vertex_buffer.hpp>

// ImGui
#include <imgui.h>

int main(int argc, char** argv)
{
  tyl::app::Loop app{"tyl", tyl::Vec2i{720, 720}};

  tyl::ecs::Registry registry;

  const auto player_entity = registry.create();
  registry.emplace<tyl::camera::TopDown>(player_entity);
  registry.emplace<std::string>(player_entity, "player");

  tyl::tile_map::create(
    registry,
    tyl::tile_map::Options{.dimensions = tyl::Vec2i{10, 10},
                           .tile_size = tyl::Vec2i{16, 16},
                           .transform = tyl::Transform::translation(tyl::Vec2f{-80, -80}),
                           .altas_texture_entity = tyl::graphics::load_texture(registry, "resources/test/poke-gba.png"),
                           .shader_entity = tyl::tile_map::create_default_shader(registry),
                           .initial_tile_id = 1});

  return app.run([&](const tyl::app::State& state) -> bool {
    ImGui::Begin("engine-debug");

    auto view = registry.view<tyl::camera::TopDown, std::string>();
    for (const auto e : view)
    {
      auto [camera, name] = view.get<tyl::camera::TopDown, std::string>(e);

      ImGui::PushID(name.c_str());
      ImGui::Text(name.c_str());
      ImGui::SliderFloat("zoom", &camera.zoom, 0.5f, 1000.f);
      ImGui::SliderFloat("panning.x", &camera.panning[0], -10.f, 10.f);
      ImGui::SliderFloat("panning.y", &camera.panning[1], -10.f, 10.f);
      ImGui::Text(
        "cursor : %e, %e (%f, %f)",
        state.cursor_position_full_resolution.x(),
        state.cursor_position_full_resolution.y(),
        state.cursor_position_normalized.x(),
        state.cursor_position_normalized.y());
      ImGui::PopID();

      // To transform from screen -> world space
      const auto ivp_mat = view.get<tyl::camera::TopDown>(e).get_inverse_view_projection_matrix(state.viewport_size);

      // Get view-projection matrix from its inverse
      const auto vp_mat = ivp_mat.inverse();

      // Runder all tilemap sectors
      tyl::tile_map::render(registry, vp_mat);
    }
    ImGui::End();
    return true;
  });
}
