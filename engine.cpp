// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/engine/app.hpp>
#include <tyl/engine/camera.hpp>
#include <tyl/engine/geometry.hpp>
#include <tyl/engine/tile_map.hpp>
#include <tyl/engine/tile_map_rendering.hpp>
#include <tyl/graphics/image.hpp>
#include <tyl/graphics/shader.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/vertex_buffer.hpp>

// ImGui
#include <imgui.h>

int main(int argc, char** argv)
{
  tyl::engine::App app{"tyl", tyl::engine::ViewportSize{720, 720}};

  using namespace tyl::graphics;

  const auto image = Image::load_from_file("resources/test/poke-gba.png");

  const Texture texture{image};
  texture.bind(0);

  entt::registry registry;

  const auto tile_map_shader_entity = tyl::engine::create_tile_map_default_shader(registry);

  const auto player_entity = registry.create();
  registry.emplace<tyl::engine::TopDownCamera>(player_entity);

  const auto tile_map_entity = tyl::engine::create_tile_map(
    registry,
    tyl::Vec2i{10, 10},
    tyl::engine::TileSizePx{16, 16},
    tyl::engine::Transform::translation(tyl::Vec2f{-80.f, -80.f}),
    1);

  tyl::engine::add_tile_map_render_data(
    registry,
    tile_map_entity,
    tyl::engine::TileAtlasSizePx{image.rows(), image.cols()},
    texture,
    registry.get<Shader>(tile_map_shader_entity));

  return app.run([&](const tyl::engine::WindowProperties& window_props) -> bool {
    // ImGui::Begin("engine-debug");
    // ImGui::SliderFloat("zoom", &camera.zoom, 0.1f, 10.f);
    // ImGui::SliderFloat("panning.x", &camera.panning[0], -10.f, 10.f);
    // ImGui::SliderFloat("panning.y", &camera.panning[1], -10.f, 10.f);
    // ImGui::Text(
    //   "cursor : %e, %e (%f, %f : %f, %f)",
    //   window_props.cursor_position_full_resolution.x(),
    //   window_props.cursor_position_full_resolution.y(),
    //   window_props.cursor_position_normalized.x(),
    //   window_props.cursor_position_normalized.y(),
    //   window_props.cursor_position_world.x(),
    //   window_props.cursor_position_world.y());
    // ImGui::End();

    auto view = registry.view<tyl::engine::TopDownCamera>();
    for (const auto e : view)
    {
      // To transform from screen -> world space
      const auto ivpm = tyl::engine::make_inverse_view_projection_matrix(
        registry.get<tyl::engine::TopDownCamera>(e), window_props.viewport_size);

      // To transform from world -> screen space
      const auto vpm = tyl::engine::make_view_projection_matrix(ivpm);

      // Runder all tilemap sectors
      tyl::engine::render_tile_maps(registry, vpm);
    }

    return true;
  });
}
