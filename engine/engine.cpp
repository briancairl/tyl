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
#include <tyl/engine/widget/asset_management.hpp>
#include <tyl/engine/widget/io.hpp>
#include <tyl/engine/widget/perf_monitor.hpp>
#include <tyl/engine/widget/texture_browser.hpp>
#include <tyl/engine/widget/tileset_creator.hpp>
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

  auto asset_management = AssetManagement::create({});
  if (!asset_management.has_value())
  {
    return 1;
  }

  auto perf_monitor = PerfMonitor::create({});
  if (!perf_monitor.has_value())
  {
    return 1;
  }

  auto tileset_creator = TileSetCreator::create({});
  if (!tileset_creator.has_value())
  {
    return 1;
  }

  auto texture_browser = TextureBrowser::create({});
  if (!texture_browser.has_value())
  {
    return 1;
  }

  Scene scene;
  WidgetSharedState shared;
  WidgetResources resources;

  if (const auto path = (working_directory / "scene.bin"); std::filesystem::exists(path))
  {
    serialization::file_istream ifs{path};
    serialization::binary_iarchive iar{ifs};
    iar >> serialization::named{"scene", scene};
  }

  load(*asset_management, working_directory / "asset_management.bin");
  load(*perf_monitor, working_directory / "perf_monitor.bin");
  load(*tileset_creator, working_directory / "tileset_creator.bin");
  load(*texture_browser, working_directory / "texture_browser.bin");


  auto on_update = [&](WindowState& window_state) {
    resources.gui_context = window_state.gui_context;
    resources.now = window_state.now;
    std::swap(window_state.drop_payloads, resources.drop_payloads);
    std::swap(window_state.drop_cursor_position, resources.drop_cursor_position);
    window_state.drop_payloads.clear();

    asset_management->update(scene, shared, resources);
    perf_monitor->update(scene, shared, resources);
    tileset_creator->update(scene, shared, resources);
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
  save(*perf_monitor, working_directory / "perf_monitor.bin");
  save(*tileset_creator, working_directory / "tileset_creator.bin");
  save(*texture_browser, working_directory / "texture_browser.bin");

  return retcode;
}
