/**
 * @copyright 2022-present Brian Cairl
 *
 * @file engine.cpp
 */

// C++ Standard Library
#include <iostream>

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/engine/widget.hpp>
#include <tyl/engine/widget_perf_monitor.hpp>
#include <tyl/engine/widget_texture_browser.hpp>
#include <tyl/engine/widget_tileset_creator.hpp>
#include <tyl/engine/window.hpp>

using namespace tyl;
using namespace tyl::engine;

int main(int argc, char** argv)
{
  auto window = Window::create(
    {.initial_window_height = 1000,
     .initial_window_width = 1500,
     .window_title = "tyl",
     .enable_vsync = true,
     .behavior = {}});

  if (!window.has_value())
  {
    std::cerr << window.error() << std::endl;
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

  while (true)
  {
    switch (window->update(on_update))
    {
    case WindowStatus::kRunning:
      continue;
    case WindowStatus::kClosing:
      return 0;
    case WindowStatus::kUpdateFailure:
      return 1;
    }
  }
  return 0;
}
