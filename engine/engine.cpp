/**
 * @copyright 2022-present Brian Cairl
 *
 * @file engine.cpp
 */

// C++ Standard Library
#include <iostream>

// Tyl
#include <tyl/engine/window.hpp>

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

  auto on_update = [](const WindowState& state) { return true; };

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
