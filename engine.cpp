// C++ Standard Library
#include <string>

// Tyl
#include <tyl/app/loop.hpp>
#include <tyl/assert.hpp>
#include <tyl/components.hpp>
#include <tyl/ecs.hpp>
#include <tyl/filesystem.hpp>
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

  const auto bg_texture_id = graphics::create_texture(registry, "resources/test/poke-gba.png");

  const auto bg_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, bg_texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{2, 2},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{16, 16}, Vec2i{48, 48}),
                                      .reversed = false}});


  const auto texture_id = graphics::create_texture(registry, "resources/test/poke-npc-walk.png");

  const auto walk_down_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 0}, Vec2i{240, 48}),
                                      .reversed = false}});
  const auto walk_down_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_down_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_down_sprite_id, 5.0f, true);
  const auto run_down_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_down_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_down_sprite_id, 10.0f, true);


  const auto walk_up_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 144}, Vec2i{240, 192}),
                                      .reversed = false}});
  const auto walk_up_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_up_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_up_sprite_id, 5.0f, true);
  const auto run_up_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_up_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_up_sprite_id, 10.0f, true);


  const auto walk_left_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 96}, Vec2i{240, 144}),
                                      .reversed = false}});
  const auto walk_left_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_left_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_left_sprite_id, 5.0f, true);
  const auto run_left_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_left_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_left_sprite_id, 10.0f, true);


  const auto walk_right_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry,
    ecs::ref<graphics::Texture>(registry, texture_id),
    {graphics::UniformlyDividedRegion{.subdivisions = Vec2i{4, 1},
                                      .inner_padding_px = Vec2i{0, 0},
                                      .area_px = Rect2i::corners(Vec2i{112, 48}, Vec2i{240, 96}),
                                      .reversed = false}});
  const auto walk_right_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_right_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, walk_right_sprite_id, 5.0f, true);
  const auto run_right_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, walk_right_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});
  graphics::attach_sprite_sequence(registry, run_right_sprite_id, 10.0f, true);


  const auto rest_down_tile_uv_lookup_id =
    graphics::create_tile_uv_lookup(registry, graphics::ref_tile_uv_lookup(registry, walk_down_tile_uv_lookup_id), {0});
  const auto rest_down_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, rest_down_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_up_tile_uv_lookup_id =
    graphics::create_tile_uv_lookup(registry, graphics::ref_tile_uv_lookup(registry, walk_up_tile_uv_lookup_id), {0});
  const auto rest_up_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, rest_up_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_left_tile_uv_lookup_id =
    graphics::create_tile_uv_lookup(registry, graphics::ref_tile_uv_lookup(registry, walk_left_tile_uv_lookup_id), {0});
  const auto rest_left_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, rest_left_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});


  const auto rest_right_tile_uv_lookup_id = graphics::create_tile_uv_lookup(
    registry, graphics::ref_tile_uv_lookup(registry, walk_right_tile_uv_lookup_id), {0});
  const auto rest_right_sprite_id = graphics::create_sprite(
    registry, graphics::ref_tile_uv_lookup(registry, rest_right_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{30, 32}});

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
    // const auto t_id =
    graphics::create_tiled(
      registry, graphics::ref_tile_uv_lookup(registry, bg_tile_uv_lookup_id), Rect2D{Vec2f{0, 0}, Vec2f{192, 192}});
    // registry.emplace<graphics::BoundingBoxColor>(t_id, 1, 0, 1, 1);
  }

  {
    // const auto t_id =
    graphics::create_tiled(
      registry, graphics::ref_tile_uv_lookup(registry, bg_tile_uv_lookup_id), Rect2D{Vec2f{192, 0}, Vec2f{192, 192}});
    // registry.emplace<graphics::BoundingBoxColor>(t_id, 1, 1, 0, 1);
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

    static std::optional<ecs::entity> selected_entity{std::nullopt};
    {
      using namespace graphics;
      registry.view<InverseViewProjection, ViewportRect>().each(
        [&registry, &user_input](const InverseViewProjection& inv_view_projection, const ViewportRect& view_rect) {
          const Vec2f cursor_world = inv_view_projection.block<2, 2>(0, 0) * user_input.cursor_position_normalized +
            inv_view_projection.block<2, 1>(0, 2);
          registry.view<Rect2D>().each(
            [&registry, &user_input, &cursor_world](const entt::entity id, const Rect2D& rect) {
              if (rect.within(cursor_world))
              {
                if (user_input.is_pressed(app::UserInput::LMB))
                {
                  selected_entity.emplace(id);
                }
                registry.emplace_or_replace<graphics::BoundingBoxColor>(id, 1.f, 0.f, 0.f, 1.f);
              }
              else
              {
                registry.remove_if_exists<graphics::BoundingBoxColor>(id);
              }
            });
        });
    }


    {
      using namespace graphics;

      if (selected_entity and registry.has<TiledRegion>(selected_entity.value()))
      {
        auto& region = registry.get<TiledRegion>(selected_entity.value());
        ImGui::Begin("tile-editor");
        ImGui::PushItemWidth(100);
        for (int i = 0; i < TiledRegion::S; ++i)
        {
          for (int j = 0; j < TiledRegion::S; ++j)
          {
            const std::size_t lidx = i * TiledRegion::S + j;
            ImGui::PushID(lidx);
            int id_int = region.ids[lidx];
            ImGui::InputInt("##id", &id_int);
            region.ids[lidx] = std::max(0, id_int);
            ImGui::PopID();

            if (j + 1 < TiledRegion::S)
            {
              ImGui::SameLine();
            }
          }
        }
        ImGui::PopItemWidth();
        ImGui::End();
      }

      ImGui::Begin("active-textures");
      {
        static float texture_scaling = 0.1f;
        ImGui::SliderFloat("texture scaling", &texture_scaling, 0.1f, 2.0f);
        ImGui::BeginChild("##active-textures-table", ImVec2{0, 200});
        if (ImGui::BeginTable("##active-textures-listing", 3, ImGuiTableFlags_Resizable))
        {
          registry.view<Texture, tyl::filesystem::path>().each(
            [](const entt::entity id, const Texture& texture, const tyl::filesystem::path& path) {
              ImGui::TableNextColumn();
              ImGui::Text("%d", static_cast<int>(id));
              ImGui::TableNextColumn();
              ImGui::Text("%s", path.c_str());
              ImGui::TableNextColumn();
              ImGui::Image(
                reinterpret_cast<void*>(texture.get_id()),
                ImVec2{texture.size().x() * texture_scaling, texture.size().y() * texture_scaling});
            });
          ImGui::EndTable();
        }
        ImGui::EndChild();
      }

      {
        static float texture_tile_size = 100.f;
        ImGui::SliderFloat("tile scaling", &texture_tile_size, 50, 500);
        ImGui::BeginChild("##active-texture-lookups-table", ImVec2{0, 400});
        if (ImGui::BeginTable("##active-texture-lookups-listing", 2, ImGuiTableFlags_Resizable))
        {
          registry.view<TileUVLookup, ecs::Ref<Texture>>().each(
            [](const entt::entity id, const TileUVLookup& tile_uv_lookup, const ecs::Ref<Texture>& r_texture) {
              ImGui::TableNextColumn();
              ImGui::Text("%d", static_cast<int>(id));
              ImGui::TableNextColumn();
              ImGui::PushID((*r_texture).get_id());
              int index_counter = 0;
              for (const auto& bounds : tile_uv_lookup)
              {
                ImGui::Text("[%d]", index_counter++);
                ImGui::SameLine();
                ImGui::Image(
                  reinterpret_cast<void*>((*r_texture).get_id()),
                  ImVec2{texture_tile_size, texture_tile_size},
                  ImVec2{bounds[0], bounds[1]},
                  ImVec2{bounds[0] + bounds[2], bounds[1] + bounds[3]},
                  ImVec4{1, 1, 1, 1},
                  ImVec4{1, 1, 1, 1});
                ImGui::SameLine();
              }
              ImGui::PopID();
            });
          ImGui::EndTable();
        }
        ImGui::EndChild();
      }
      ImGui::End();
    }

    return true;
  });
}
