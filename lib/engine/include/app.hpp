/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

// Tyl
#include <tyl/render/viewport.hpp>

namespace tyl::engine
{

class App
{
public:
  App(const char* name, const render::ViewportSize& size);

  ~App();

  int run(const std::function<bool(const render::ViewportSize&)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  render::ViewportSize window_size_;
};

}  // namespace tyl::engine
