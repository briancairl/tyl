/**
 * @copyright 2022-present Brian Cairl
 *
 * @file editor.cpp
 */


// C++ Standard Library
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_map>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

// ImGuiFileDialogue
#include <ImGuiFileDialog.h>

// EnTT
#include <entt/entt.hpp>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/engine/core/app.hpp>
#include <tyl/engine/core/asset.hpp>
#include <tyl/engine/core/resources.hpp>
#include <tyl/engine/graphics/primitives_renderer.hpp>
#include <tyl/engine/graphics/types.hpp>
#include <tyl/engine/widgets/drag_and_drop.hpp>
#include <tyl/engine/widgets/text_asset_manager.hpp>
#include <tyl/engine/widgets/texture_asset_manager.hpp>
#include <tyl/engine/widgets/tileset_creator.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/render_target.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/utility/expected.hpp>

using namespace tyl::engine;
using namespace tyl::graphics;


int main(int argc, char** argv)
{
  auto app = tyl::engine::core::App::create({
    .initial_window_height = 1000,
    .initial_window_width = 1500,
    .window_title = "engine",
    .enable_vsync = true,
  });
  if (!app.has_value())
  {
    std::cerr << app.error() << std::endl;
    return 1;
  }

  core::Resources resources;

  {
    const auto id = resources.registry.create();

    resources.registry.emplace<graphics::DrawType::LineStrip>(id);
    resources.registry.emplace<graphics::VertexColor>(id, 1.0f, 0.0f, 0.0f, 1.0f);

    {
      auto& vertices = resources.registry.emplace<graphics::VertexList2D>(id);
      vertices.emplace_back(+0.5f, +0.0f);
      vertices.emplace_back(+0.5f, +0.5f);
      vertices.emplace_back(-0.5f, -0.0f);
      vertices.emplace_back(-0.5f, -0.5f);
    }
  }


  auto& manipulated_point = [&registry = resources.registry]() -> tyl::Vec2f& {
    const auto id = registry.create();

    registry.emplace<graphics::DrawType::LineStrip>(id);
    registry.emplace<graphics::VertexColor>(id, 1.0f, 0.0f, 1.0f, 1.0f);

    {
      auto& vertices = registry.emplace<graphics::VertexList2D>(id);
      vertices.emplace_back(+0.8f, +0.0f);
      vertices.emplace_back(+0.8f, +0.8f);
      vertices.emplace_back(-0.8f, -0.0f);
      vertices.emplace_back(-0.8f, -0.8f);
    }
    return registry.get<graphics::VertexList2D>(id).back();
  }();


  auto primitives_renderer = graphics::PrimitivesRenderer::create({.max_vertex_count = 100});
  if (!primitives_renderer.has_value())
  {
    return 1;
  }

  graphics::TopDownCamera2D camera{
    .translation = {-0.0f, 0.0f},
    .scaling = 1.0f,
  };

  auto tileset_creator = widgets::TilesetCreator::create({});
  if (!tileset_creator.has_value())
  {
    return 1;
  }

  auto texture_asset_manager = widgets::TextureAssetManager::create({});
  if (!texture_asset_manager.has_value())
  {
    return 1;
  }

  auto text_asset_manager = widgets::TextAssetManager::create({});
  if (!text_asset_manager.has_value())
  {
    return 1;
  }

  auto drag_and_drop = widgets::DragAndDrop::create({});
  if (!drag_and_drop.has_value())
  {
    return 1;
  }


  const auto update_callback = [&](const tyl::engine::core::App::State& app_state) {
    using Key = tyl::engine::core::KeyInfo;

    if (app_state.cursor_scroll.is_valid())
    {
      camera.scaling -= app_state.cursor_scroll.get()[0] * 0.1f;
      camera.scaling = std::max(0.1f, camera.scaling);
    }

    if (app_state.key_info[Key::W].is_held())
    {
      camera.translation.x() += 0.1;
    }

    if (app_state.key_info[Key::S].is_held())
    {
      camera.translation.x() -= 0.1;
    }

    if (app_state.key_info[Key::A].is_held())
    {
      camera.translation.y() += 0.1;
    }

    if (app_state.key_info[Key::D].is_held())
    {
      camera.translation.y() -= 0.1;
    }

    const tyl::Mat3f cmat_inv = graphics::to_camera_inverse_matrix(camera, app_state.window_size.cast<float>());
    const tyl::Mat3f cmat = cmat_inv.inverse();

    const tyl::Vec2f cursor_position_cmat =
      cmat_inv.block<2, 2>(0, 0) * app_state.cursor_position_normalized + cmat_inv.col(2).head<2>();
    manipulated_point = cursor_position_cmat;

    primitives_renderer->draw(cmat, resources.registry);
    tileset_creator->update(app_state.imgui_context, resources);
    texture_asset_manager->update(app_state.imgui_context, resources);
    text_asset_manager->update(app_state.imgui_context, resources);
    drag_and_drop->update(app_state.imgui_context, resources);

    tyl::engine::core::asset::update(resources.registry);

    return true;
  };

  while (app->update(resources.registry, update_callback))
  {}
  return 0;
}
