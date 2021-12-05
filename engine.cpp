// C++ Standard Library
#include <string>

// Tyl
#include <tyl/app/loop.hpp>
#include <tyl/assert.hpp>
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/time.hpp>

// ImGui
#include <imgui.h>

using namespace tyl;

int main(int argc, char** argv)
{
  app::Loop loop{"tyl", Vec2i{720, 720}};

  graphics::device::enable_error_logs();

  ecs::registry registry;

  {
    const auto texture_id = registry.create();
    const auto& texture =
      registry.emplace<graphics::Texture>(texture_id, graphics::load_texture("resources/test/patrick-run.png"));

    auto& uv_lookup = registry.emplace<graphics::TileUVLookup>(texture_id);

    uv_lookup.update(
      *texture(registry, texture_id),
      graphics::UniformlyDividedRegion{.subdivisions = Vec2i{6, 1},
                                       .inner_padding_px = Vec2i{1, 0},
                                       .area_px = Rect2i{Vec2i{0, 0}, Vec2i{245, 50}},
                                       .reversed = false});

    uv_lookup.update(
      *texture(registry, texture_id),
      graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                       .inner_padding_px = Vec2i{1, 0},
                                       .area_px = Rect2i{Vec2i{41, 0}, Vec2i{207, 50}},
                                       .reversed = true});

    const auto animated_sprite_id = graphics::create_sprite(
      registry, texture(registry, texture_id), uv_lookup(registry, texture_id), Position2D{32, 32}, RectSize2D{16, 16});

    graphics::attach_sprite_sequence(registry, animated_sprite_id, 10.0f, true);
  }

  {
    const auto camera_id = registry.create();
    registry.emplace<graphics::CameraTopDown>(camera_id);
  }

  graphics::create_sprite_batch_renderer(registry, 10);

  return loop.run([&](graphics::Target& render_target, const app::WindowState& win_state) -> bool {
    graphics::render_sprites(registry, render_target, clock::now());

    return true;
  });
}
