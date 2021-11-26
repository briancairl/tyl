/**
 * @copyright 2021-present Brian Cairl
 *
 * @file resources.hpp
 */
#pragma once

// Tyl
#include <tyl/ecs.hpp>
#include <tyl/matrix.hpp>
#include <tyl/platform/filesystem.hpp>

namespace tyl::graphics
{

ecs::Entity load_shader(ecs::Registry& registry, const filesystem::path& path);

ecs::Entity load_texture(ecs::Registry& registry, const filesystem::path& path);

}  // namespace tyl::graphics
