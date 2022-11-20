/**
 * @copyright 2022-present Brian Cairl
 *
 * @file window.cpp
 */

#include <cstdio>
#include <tyl/window/window.hpp>

int main(int argc, char** argv)
{
  tyl::Window{{.title = "window", .size = {.height = 500, .width = 1000}}}([](const auto& s) {
    std::fprintf(
      stderr,
      "x:%f, y:%f (xn:%f, yn:%f)\n",
      s.cursor_position.x,
      s.cursor_position.y,
      s.cursor_position_normalized.x,
      s.cursor_position_normalized.y);
  });
  return 0;
}
