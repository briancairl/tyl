/**
 * @copyright 2022-present Brian Cairl
 *
 * @file engine.cpp
 */

// C++ Standard Library
#include <filesystem>

// Tyl
#include <tyl/engine/assets.hpp>
#include <tyl/engine/ecs.hpp>
#include <tyl/engine/scene.hpp>
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

  auto on_update = [&](WindowState& window_state) { return true; };

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

  return retcode;
}
