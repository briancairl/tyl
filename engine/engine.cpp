/**
 * @copyright 2022-present Brian Cairl
 *
 * @file engine.cpp
 */

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/engine/scene.hpp>
#include <tyl/engine/script/asset_management.hpp>
#include <tyl/engine/script/audio_browser.hpp>
#include <tyl/engine/script/drawing_2D.hpp>
#include <tyl/engine/script/io.hpp>
#include <tyl/engine/script/perf_monitor.hpp>
#include <tyl/engine/script/render_pipeline_2D.hpp>
#include <tyl/engine/script/scene_management.hpp>
#include <tyl/engine/script/texture_browser.hpp>
#include <tyl/engine/script/tile_map_creator.hpp>
#include <tyl/engine/script/tile_set_creator.hpp>
#include <tyl/engine/window.hpp>
#include <tyl/serialization/binary_archive.hpp>
#include <tyl/serialization/file_stream.hpp>
#include <tyl/serialization/named.hpp>

using namespace tyl;
using namespace tyl::engine;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::fprintf(stderr, "%s\n", "[ERROR] REQUIRES:   ./engine <savefile> ");
    return 1;
  }

  auto window = Window::create(
    {.initial_window_height = 1000,
     .initial_window_width = 1500,
     .window_title = "tyl",
     .enable_vsync = true,
     .runtime = {}});

  if (!window.has_value())
  {
    std::fprintf(stderr, "%s\n", "[ERROR] Failed to create application.");
    return 1;
  }

  const std::filesystem::path working_directory{argv[1]};
  if (std::filesystem::create_directories(working_directory))
  {
    std::fprintf(stderr, "[INFO] created working directory: %s\n", working_directory.string().c_str());
  }

  auto audio_browser = AudioBrowser::create({});
  if (!audio_browser.has_value())
  {
    return 1;
  }

  auto asset_management = AssetManagement::create({});
  if (!asset_management.has_value())
  {
    return 1;
  }

  auto drawing_2D = Drawing2D::create({});
  if (!drawing_2D.has_value())
  {
    return 1;
  }

  auto perf_monitor = PerfMonitor::create({});
  if (!perf_monitor.has_value())
  {
    return 1;
  }

  auto render_pipeline_2D = RenderPipeline2D::create({});
  if (!render_pipeline_2D.has_value())
  {
    return 1;
  }

  auto scene_management = SceneManagement::create({});
  if (!scene_management.has_value())
  {
    return 1;
  }

  auto tile_set_creator = TileSetCreator::create({});
  if (!tile_set_creator.has_value())
  {
    return 1;
  }

  auto tile_map_creator = TileMapCreator::create({});
  if (!tile_map_creator.has_value())
  {
    return 1;
  }

  auto texture_browser = TextureBrowser::create({});
  if (!texture_browser.has_value())
  {
    return 1;
  }

  Scene scene;
  ScriptSharedState shared;
  ScriptResources resources;

  if (const auto path = (working_directory / "scene.bin"); std::filesystem::exists(path))
  {
    serialization::file_istream ifs{path};
    serialization::binary_iarchive iar{ifs};
    iar >> serialization::named{"scene", scene};
  }

  load(*asset_management, working_directory / "asset_management.bin");
  load(*audio_browser, working_directory / "audio_browser.bin");
  load(*drawing_2D, working_directory / "drawing_2D.bin");
  load(*perf_monitor, working_directory / "perf_monitor.bin");
  load(*render_pipeline_2D, working_directory / "render_pipeline_2D.bin");
  load(*scene_management, working_directory / "scene_management.bin");
  load(*tile_set_creator, working_directory / "tile_set_creator.bin");
  load(*tile_map_creator, working_directory / "tile_map_creator.bin");
  load(*texture_browser, working_directory / "texture_browser.bin");


  auto on_update = [&](WindowState& window_state) {
    resources.gui_context = window_state.gui_context;
    resources.now = window_state.now;
    resources.viewport_size = window_state.window_size.cast<float>();
    resources.viewport_cursor_position = window_state.cursor_position;
    resources.viewport_cursor_position_normalized = window_state.cursor_position_normalized;
    std::swap(window_state.drop_payloads, resources.drop_payloads);
    std::swap(window_state.drop_cursor_position, resources.drop_cursor_position);
    window_state.drop_payloads.clear();

    asset_management->update(scene, shared, resources);
    audio_browser->update(scene, shared, resources);
    drawing_2D->update(scene, shared, resources);
    perf_monitor->update(scene, shared, resources);
    render_pipeline_2D->update(scene, shared, resources);
    scene_management->update(scene, shared, resources);
    tile_set_creator->update(scene, shared, resources);
    tile_map_creator->update(scene, shared, resources);
    texture_browser->update(scene, shared, resources);
    return true;
  };

  int retcode = -1;
  while (retcode < 0)
  {
    switch (window->update(on_update))
    {
    case WindowStatus::kRunning:
      continue;
    case WindowStatus::kClosing:
      retcode = 0;
      break;
    case WindowStatus::kUpdateFailure:
      retcode = 1;
      break;
    }
  }

  {
    const auto path = (working_directory / "scene.bin");
    serialization::file_ostream ofs{path};
    serialization::binary_oarchive oar{ofs};
    oar << serialization::named{"scene", scene};
  }

  save(*asset_management, working_directory / "asset_management.bin");
  save(*audio_browser, working_directory / "audio_browser.bin");
  save(*drawing_2D, working_directory / "drawing_2D.bin");
  save(*perf_monitor, working_directory / "perf_monitor.bin");
  save(*render_pipeline_2D, working_directory / "render_pipeline_2D.bin");
  save(*scene_management, working_directory / "scene_management.bin");
  save(*tile_set_creator, working_directory / "tile_set_creator.bin");
  save(*tile_map_creator, working_directory / "tile_map_creator.bin");
  save(*texture_browser, working_directory / "texture_browser.bin");

  return retcode;
}
