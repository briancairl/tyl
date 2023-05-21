/**
 * @copyright 2022-present Brian Cairl
 *
 * @file main.cpp
 */

// C++ Standard Library
#include <cstdio>

// Tyl
#include <tyl/graphics/tilemap.hpp>
#include <tyl/window/window.hpp>

using namespace tyl;

int main(int argc, char const* argv[])
{
  Window window{{.title = "game", .size = {.height = 1000, .width = 2000}}};

  ecs::registry reg;

  {
    auto e = reg.create();
    graphics::attach_tilemap(reg, e, {0.f, 0.f}, {.rows = 10, .cols = 10, .tile_size = 5.f});
  }

  window([&](const auto& s) {

  });

  return 0;
}