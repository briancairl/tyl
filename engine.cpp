// C++ Standard Library
#include <string>

// Tyl
#include <tyl/app/loop.hpp>
#include <tyl/assert.hpp>
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/game/actor.hpp>
#include <tyl/game/tiled_region.hpp>
#include <tyl/graphics/camera.hpp>
#include <tyl/graphics/debug.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/sprite.hpp>
#include <tyl/graphics/texture.hpp>
#include <tyl/graphics/tile_uv_lookup.hpp>
#include <tyl/graphics/tiled.hpp>
#include <tyl/time.hpp>

// WIP audio
#include <tyl/audio/device/device.hpp>
#include <tyl/audio/device/listener.hpp>
#include <tyl/audio/device/sound.hpp>
#include <tyl/audio/device/source.hpp>

// ImGui
#include <imgui.h>

using namespace tyl;

#include <iostream>

int main(int argc, char** argv)
{
  app::Loop loop{"tyl", Vec2i{720, 720}};

  graphics::device::enable_error_logs();

  ecs::registry registry;

  const auto texture_id = graphics::create_texture(registry, "resources/test/poke-npc-walk.png");


  const auto walk_down_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 0}, Vec2i{240, 48}),
                                      .reversed = false}});
  const auto walk_down_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_down_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_down_sprite_id, 5.0f, true);
  const auto run_down_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_down_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_down_sprite_id, 10.0f, true);


  const auto walk_up_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 144}, Vec2i{240, 192}),
                                      .reversed = false}});
  const auto walk_up_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_up_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_up_sprite_id, 5.0f, true);
  const auto run_up_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_up_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_up_sprite_id, 10.0f, true);


  const auto walk_left_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 96}, Vec2i{240, 144}),
                                      .reversed = false}});
  const auto walk_left_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_left_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_left_sprite_id, 5.0f, true);
  const auto run_left_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_left_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_left_sprite_id, 10.0f, true);


  const auto walk_right_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 48}, Vec2i{240, 96}),
                                      .reversed = false}});
  const auto walk_right_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_right_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_right_sprite_id, 5.0f, true);
  const auto run_right_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_right_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_right_sprite_id, 10.0f, true);


  const auto rest_down_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_down_tile_uv_lookup_id),
    {0});
  const auto rest_down_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_down_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_up_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry, ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_up_tile_uv_lookup_id), {0});
  const auto rest_up_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_up_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_left_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_left_tile_uv_lookup_id),
    {0});
  const auto rest_left_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_left_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_right_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, walk_right_tile_uv_lookup_id),
    {0});
  const auto rest_right_sprite_id = graphics::create_sprite(
    registry,
    ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_right_tile_uv_lookup_id),
    Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  game::create_tiled_region(registry, Vec2f{0, 0}, Vec2f{16, 16}, Vec2i{100, 100});

  const auto player_id = game::create_actor(
    registry,
    Vec2f{0.f, 0.f},
    game::Actions{{
      rest_down_sprite_id,
      rest_up_sprite_id,
      rest_left_sprite_id,
      rest_right_sprite_id,
      walk_down_sprite_id,
      walk_up_sprite_id,
      walk_left_sprite_id,
      walk_right_sprite_id,
      run_down_sprite_id,
      run_up_sprite_id,
      run_left_sprite_id,
      run_right_sprite_id,
    }});

  const auto camera_id = graphics::create_top_down_camera(registry);

  graphics::set_camera_boundary(
    ecs::ref<graphics::TopDownCamera>(registry, camera_id), ecs::ref<Position2D>(registry, player_id), 10.0f, 0.5f);

  graphics::create_sprite_batch_renderer(registry, 100);

  graphics::create_bounding_box_batch_renderer(registry, 100);


  graphics::create_tiled_batch_renderer(registry, 1000);

  {
    const auto t_id = graphics::create_tiled(
      registry,
      ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_left_tile_uv_lookup_id),
      Rect2D{Vec2f{0, 0}, Vec2f{64, 64}});
    registry.emplace<graphics::BoundingBoxColor>(t_id, 1, 1, 0, 1);
  }

  {
    const auto t_id = graphics::create_tiled(
      registry,
      ecs::ref<graphics::TileUVLookup, ecs::Ref<graphics::Texture>>(registry, rest_right_tile_uv_lookup_id),
      Rect2D{Vec2f{64, 0}, Vec2f{64, 64}});
    registry.emplace<graphics::BoundingBoxColor>(t_id, 1, 1, 0, 1);
  }


  audio::device::Device audio_playback_device;
  audio::device::Listener audio_listener{audio_playback_device};
  audio::device::Source background_music_source;
  audio::device::Sound background_music_track{
    audio::device::load_sound_from_file("resources/test/background_mono.wav")};

  background_music_source.set_looped(true);
  const auto playback = background_music_source.play(background_music_track);

  duration t_accum = duration::zero();

  return loop.run([&](graphics::Target& render_target, const app::UserInput& user_input, const duration dt) -> bool {
    graphics::update_cameras(registry, render_target, dt);
    graphics::draw_tiles(registry, render_target, dt);
    graphics::draw_sprites(registry, render_target, dt);
    graphics::draw_bounding_boxes(registry, render_target, dt);
    game::update_actors(registry, dt);

    t_accum += dt;
    background_music_source.set_position(
      10.f * std::cos(0.75f * to_fseconds(t_accum).count()),
      10.f * std::sin(0.75f * to_fseconds(t_accum).count()),
      0.f);
    background_music_source.set_pitch_scaling(0.5f * std::cos(0.75f * to_fseconds(t_accum).count()) + 1.f);

    if (t_accum > std::chrono::seconds{10})
    {
      t_accum = duration::zero();
      playback.resume();
    }
    else if (t_accum > std::chrono::seconds{5})
    {
      playback.pause();
    }

    auto& motion = registry.get<game::Motion2D>(player_id);

    const float speed = user_input.is_down(app::UserInput::Sprint) ? 50.0f : 25.0f;

    if (user_input.is_down(app::UserInput::MoveUp))
    {
      motion.y() = speed;
    }
    else if (user_input.is_down(app::UserInput::MoveDown))
    {
      motion.y() = -speed;
    }

    if (user_input.is_down(app::UserInput::MoveRight))
    {
      motion.x() = speed;
    }
    else if (user_input.is_down(app::UserInput::MoveLeft))
    {
      motion.x() = -speed;
    }
    return true;
  });
}
