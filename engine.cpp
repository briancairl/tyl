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

  registry.set<tyl::engine::TopDownCamera>();
  registry.set<tyl::engine::UnitConversion>(tyl::engine::UnitConversion{.pixels_per_meter = 100.f});

  const auto tile_map_entity = tyl::engine::create_tile_map(
    registry,
    tyl::engine::TileMapDimensions{10, 10},
    tyl::engine::TileSizePx{16, 16},
    tyl::engine::Transform::Identity(),
    1);

  tyl::engine::add_tile_map_render_data(
    registry,
    tile_map_entity,
    tyl::engine::TileAtlasSizePx{image.rows(), image.cols()},
    texture,
    registry.get<Shader>(tile_map_shader_entity));

  return app.run([&](const tyl::engine::WindowProperties& window_props) -> bool {
    auto& camera = registry.ctx<tyl::engine::TopDownCamera>();

    const auto inverse_view_matrix = tyl::engine::make_inverse_view_projection_matrix(
      camera, window_props.viewport_size, registry.ctx<tyl::engine::UnitConversion>());

    ImGui::Begin("engine-debug");
    ImGui::SliderFloat("zoom", &camera.zoom, 0.1f, 10.f);
    ImGui::SliderFloat("panning.x", &camera.panning[0], -10.f, 10.f);
    ImGui::SliderFloat("panning.y", &camera.panning[1], -10.f, 10.f);
    {
      const auto cursor_position = window_props.get_cursor_position();
      const auto cursor_position_normalized = window_props.get_cursor_position_normalized();
      const auto cursor_position_world =
        (inverse_view_matrix.block<2, 2>(0, 0) * cursor_position_normalized + inverse_view_matrix.block<2, 1>(0, 2))
          .eval();
      ImGui::Text(
        "cursor : %d, %d (%f, %f : %f, %f)",
        cursor_position.x(),
        cursor_position.y(),
        cursor_position_normalized.x(),
        cursor_position_normalized.y(),
        cursor_position_world.x(),
        cursor_position_world.y());
    }
    ImGui::End();

    tyl::engine::render_tile_maps(registry, inverse_view_matrix.inverse());

    return true;
  });
}
