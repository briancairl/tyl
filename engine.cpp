// C++ Standard Library
#include <string>

// Tyl
#include <tyl/app/loop.hpp>
#include <tyl/assert.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>

// ImGui
#include <imgui.h>

using namespace tyl;

int main(int argc, char** argv)
{
  app::Loop app{"tyl", Vec2i{720, 720}};

  graphics::device::enable_error_logs();

  ecs::registry registry;

  {
    const auto texture_id = registry.create();
    const auto& texture =
      registry.emplace<graphics::Texture>(texture_id, graphics::load_texture("resources/test/poke-gba.png"));
    const auto& uv_lookup =
      registry.emplace<graphics::TileUVLookup>(texture_id, Size2i{16, 16}, texture, Rect2i{Vec2i{0, 0}, Vec2i{64, 64}});
    const auto sprite_id = graphics::create_sprite(
      registry,
      texture(registry, texture_id),
      uv_lookup(registry, texture_id),
      graphics::Position{0, 0},
      graphics::RectSize{16, 16});
    registry.get<graphics::SpriteTileID>(sprite_id).id = 2;
  }

  {
    const auto camera_id = registry.create();
    registry.emplace<graphics::CameraTopDown>(camera_id);
  }

  graphics::create_sprite_batch_renderer(registry, 10);

  return app.run([&](const tyl::app::State& state) -> bool {
    graphics::render_sprites(registry, state.viewport_size);

    return true;
  });
}
