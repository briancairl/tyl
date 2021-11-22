/**
 * @copyright 2021-present Brian Cairl
 *
 * @file app.hpp
 */
#pragma once

// C++ Standard Library
#include <functional>

// Tyl
#include <tyl/engine/window.hpp>

namespace tyl::engine
{

class App
{
public:
  App(const char* name, const ViewportSize& size);

  ~App();

  int run(const std::function<bool(const WindowProperties&)>& loop_fn);

private:
  const char* window_name_;
  void* window_ctx_;
  engine::WindowProperties window_properties_;
};

}  // namespace tyl::engine
