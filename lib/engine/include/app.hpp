/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

namespace tyl::engine
{

struct WindowSize
{
  int height_px;
  int width_px;
};

class App
{
public:
  App(const char* name, const WindowSize& size);

  ~App();

  int run(const std::function<bool(const WindowSize&)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  WindowSize window_size_;
};

}  // namespace tyl::engine
