// C++ Standard Library
#include <string>

// Tyl
#include <tyl/actor/actor.hpp>
#include <tyl/actor/components.hpp>
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


  const auto player_id = actor::create_actor(
    registry,
    Vec2f{0.f, 0.f},
    actor::Actions{{
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

  graphics::create_sprite_batch_renderer(registry, 10);


  audio::device::Device audio_playback_device;
  audio::device::Listener audio_listener{audio_playback_device};
  audio::device::Source background_music_source;
  audio::device::Sound background_music_track{
    audio::device::load_sound_from_file("resources/test/background_mono.wav")};

  background_music_source.set_looped(true);
  const auto playback = background_music_source.play(background_music_track);

  duration t_accum = duration::zero();

  return loop.run([&](graphics::Target& render_target, const app::WindowState& win_state, const duration dt) -> bool {
    graphics::update_cameras(registry, render_target, dt);
    graphics::draw_sprites(registry, render_target, dt);
    actor::update(registry, dt);

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

    auto& motion = registry.get<actor::Motion2D>(player_id);

    const float speed = (win_state.input_down_mask & app::WindowState::Sprint) ? 50.0f : 25.0f;

    if (win_state.is_down(app::WindowState::MoveUp))
    {
      motion.y() = speed;
    }
    else if (win_state.is_down(app::WindowState::MoveDown))
    {
      motion.y() = -speed;
    }

    if (win_state.is_down(app::WindowState::MoveRight))
    {
      motion.x() = speed;
    }
    else if (win_state.is_down(app::WindowState::MoveLeft))
    {
      motion.x() = -speed;
    }
    return true;
  });
}
