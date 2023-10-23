/**
 * @copyright 2022-present Brian Cairl
 *
 * @file engine.cpp
 */

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/engine/widget.hpp>
#include <tyl/engine/widget_perf_monitor.hpp>
#include <tyl/engine/widget_texture_browser.hpp>
#include <tyl/engine/widget_tileset_creator.hpp>
#include <tyl/engine/window.hpp>
#include <tyl/serialization/binary_iarchive.hpp>
#include <tyl/serialization/binary_oarchive.hpp>
#include <tyl/serialization/file_istream.hpp>
#include <tyl/serialization/file_ostream.hpp>

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

  Registry registry;
  WidgetSharedState shared;
  WidgetResources resources;

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

  if (std::filesystem::exists(argv[1]))
  {
    serialization::file_istream fs{argv[1]};
    serialization::binary_iarchive iar{fs};
    perf_monitor->load(iar, registry);
    tileset_creator->load(iar, registry);
    texture_browser->load(iar, registry);
  }

  auto on_update = [&](WindowState& window_state) {
    resources.gui_context = window_state.gui_context;
    resources.now = window_state.now;
    std::swap(window_state.drop_payloads, resources.drop_payloads);
    std::swap(window_state.drop_cursor_position, resources.drop_cursor_position);
    window_state.drop_payloads.clear();

    perf_monitor->update(registry, shared, resources);
    tileset_creator->update(registry, shared, resources);
    texture_browser->update(registry, shared, resources);
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
    serialization::file_ostream ofs{argv[1]};
    serialization::binary_oarchive oar{ofs};
    perf_monitor->save(oar, registry);
    tileset_creator->save(oar, registry);
    texture_browser->save(oar, registry);
  }

  return retcode;
}
